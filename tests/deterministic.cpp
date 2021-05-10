//
// Copyright (c) 2017 CNRS, NYU, MPI Tübingen
//
// This file is part of tsid
// tsid is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
// tsid is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// tsid If not, see
// <http://www.gnu.org/licenses/>.
//

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>

#include <tsid/contacts/contact-6d.hpp>
#include <tsid/contacts/contact-point.hpp>
#include <tsid/formulations/inverse-dynamics-formulation-acc-force.hpp>
#include <tsid/tasks/task-com-equality.hpp>
#include <tsid/tasks/task-se3-equality.hpp>
#include <tsid/tasks/task-joint-posture.hpp>
#include <tsid/tasks/task-joint-bounds.hpp>
#include <tsid/trajectories/trajectory-euclidian.hpp>
#include <tsid/solvers/solver-HQP-factory.hxx>
#include <tsid/solvers/utils.hpp>
#include <tsid/utils/stop-watch.hpp>
#include <tsid/utils/statistics.hpp>
#include <tsid/math/utils.hpp>

#include <pinocchio/algorithm/joint-configuration.hpp> // integrate
#include <pinocchio/parsers/srdf.hpp>

using namespace tsid;
using namespace tsid::trajectories;
using namespace tsid::math;
using namespace tsid::contacts;
using namespace tsid::tasks;
using namespace tsid::solvers;
using namespace tsid::robots;
using namespace std;


const string romeo_model_path = TSID_SOURCE_DIR"/models/romeo";
const string quadruped_model_path = TSID_SOURCE_DIR"/models/quadruped";

#ifndef NDEBUG
const int max_it = 5000;
#else
const int max_it = 5000;
#endif

BOOST_AUTO_TEST_SUITE ( BOOST_TEST_MODULE )

class StandardRomeoInvDynCtrl
{
  public:
  static const double lxp;
  static const double lxn;
  static const double lyp;
  static const double lyn;
  static const double lz;
  static const double mu;
  static const double fMin;
  static const double fMax;
  static const std::string rf_frame_name;
  static const std::string lf_frame_name;
  static const Vector3 contactNormal;
  static const double w_com;
  static const double w_rf;
  static const double w_lf;
  static const double w_posture;
  static const double w_forceReg;
  static const double kp_contact;
  static const double kp_com;
  static const double kp_lf;
  static const double kp_rf;
  static const double kp_posture;
  double t;

  std::shared_ptr<RobotWrapper> robot;
  std::shared_ptr<InverseDynamicsFormulationAccForce> tsid;
  std::shared_ptr<Contact6d> contactRF;
  std::shared_ptr<Contact6d> contactLF;
  std::shared_ptr<TaskComEquality> comTask;
  std::shared_ptr<TaskSE3Equality> leftFootTask;
  std::shared_ptr<TaskSE3Equality> rightFootTask;
  std::shared_ptr<TaskJointPosture> postureTask;
  std::shared_ptr<TaskJointBounds> jointBoundsTask;
  Vector q;
  Vector v;
  pinocchio::SE3 H_rf_ref;
  pinocchio::SE3 H_lf_ref;

  StandardRomeoInvDynCtrl(double dt) : t(0.)
  {
    vector<string> package_dirs;
    package_dirs.push_back(romeo_model_path);
    const string urdfFileName = package_dirs[0] + "/urdf/romeo.urdf";
    robot = std::make_shared<RobotWrapper>(urdfFileName, package_dirs, pinocchio::JointModelFreeFlyer());
    
    const string srdfFileName = package_dirs[0] + "/srdf/romeo_collision.srdf";

    pinocchio::srdf::loadReferenceConfigurations(robot->model(),srdfFileName,false);
    
    const unsigned int nv = robot->nv();
    q = neutral(robot->model());
    std::cout << "q: " << q.transpose() << std::endl;
    q(2) += 0.84;
    v = Vector::Zero(nv);
    BOOST_REQUIRE(robot->model().existFrame(rf_frame_name));
    BOOST_REQUIRE(robot->model().existFrame(lf_frame_name));

    // Create the inverse-dynamics formulation
    tsid = std::make_shared<InverseDynamicsFormulationAccForce>("tsid", *robot);
    tsid->computeProblemData(t, q, v);
    pinocchio::Data & data = tsid->data();

    /////////////////////////////////////////////////////////////////

    leftFootTask = std::make_shared<TaskSE3Equality>("left-foot",
                                                          *robot,
                                                          lf_frame_name);
    leftFootTask->Kp(kp_lf*Vector::Ones(6));
    leftFootTask->Kd(2.0*leftFootTask->Kp().cwiseSqrt());
    pinocchio::SE3 lf_ref = robot->position(tsid->data(),
                                      robot->model().getJointId(lf_frame_name));
    tsid->addMotionTask(*leftFootTask, w_lf, 1);

    TrajectorySample ls(12, 6);
    SE3ToVector(lf_ref, ls.pos);
    leftFootTask->setReference(ls);


    /////////////////////////////////////////////////////////////////

    rightFootTask = std::make_shared<TaskSE3Equality>("right-foot",
                                                          *robot,
                                                          rf_frame_name);
    rightFootTask->Kp(kp_rf*Vector::Ones(6));
    rightFootTask->Kd(2.0*rightFootTask->Kp().cwiseSqrt());
    pinocchio::SE3 rf_ref = robot->position(tsid->data(),
                                      robot->model().getJointId(rf_frame_name));
    tsid->addMotionTask(*rightFootTask, w_rf, 1);

    TrajectorySample rs(12, 6);
    SE3ToVector(rf_ref, rs.pos);
    rightFootTask->setReference(rs);

    /////////////////////////////////////////////////////////////////

    // Add the contact constraints
    Matrix3x contactPoints(3,4);
    contactPoints << -lxn, -lxn, +lxp, +lxp,
                     -lyn, +lyp, -lyn, +lyp,
                      lz,  lz,  lz,  lz;
    contactRF = std::make_shared<Contact6d>("contact_rfoot", *robot, rf_frame_name,
                                           contactPoints, contactNormal,
                                           mu, fMin, fMax);
    contactRF->Kp(kp_contact*Vector::Ones(6));
    contactRF->Kd(2.0*contactRF->Kp().cwiseSqrt());
    H_rf_ref = robot->position(data, robot->model().getJointId(rf_frame_name));
    contactRF->setReference(H_rf_ref);
    tsid->addRigidContact(*contactRF, w_forceReg);

    contactLF = std::make_shared<Contact6d>("contact_lfoot", *robot, lf_frame_name,
                                            contactPoints, contactNormal,
                                            mu, fMin, fMax);
    contactLF->Kp(kp_contact*Vector::Ones(6));
    contactLF->Kd(2.0*contactLF->Kp().cwiseSqrt());
    H_lf_ref = robot->position(data, robot->model().getJointId(lf_frame_name));
    contactLF->setReference(H_lf_ref);
    tsid->addRigidContact(*contactLF, w_forceReg);

    // Add the com task
    comTask = std::make_shared<TaskComEquality>("task-com", *robot);
    comTask->Kp(kp_com*Vector::Ones(3));
    comTask->Kd(2.0*comTask->Kp().cwiseSqrt());
    tsid->addMotionTask(*comTask, w_com, 1);

    // Add the posture task
    postureTask = std::make_shared<TaskJointPosture>("task-posture", *robot);
    postureTask->Kp(kp_posture*Vector::Ones(nv-6));
    postureTask->Kd(2.0*postureTask->Kp().cwiseSqrt());
    tsid->addMotionTask(*postureTask, w_posture, 1);

    // Add the joint bounds task
    jointBoundsTask = std::make_shared<TaskJointBounds>("task-joint-bounds", *robot, dt);
    Vector dq_max = 10*Vector::Ones(robot->na());
    Vector dq_min = -dq_max;
    jointBoundsTask->setVelocityBounds(dq_min, dq_max);
    tsid->addMotionTask(*jointBoundsTask, 1.0, 0);

    /////////////////////////////////////////////////////////////////

    // rightFootTask = std::make_shared<TaskSE3Equality>("right-foot",
    //                                                       *robot,
    //                                                       rf_frame_name);
    // rightFootTask->Kp(kp_rf*Vector::Ones(6));
    // rightFootTask->Kd(2.0*rightFootTask->Kp().cwiseSqrt());
    // pinocchio::SE3 rf_ref = robot->position(tsid->data(),
    //                                   robot->model().getJointId(rf_frame_name));
    // tsid->addMotionTask(*rightFootTask, w_rf, 1);

    // TrajectorySample rs(12, 6);
    // SE3ToVector(rf_ref, rs.pos);
    // rightFootTask->setReference(rs);

    /////////////////////////////////////////////////////////////////

    // leftFootTask = std::make_shared<TaskSE3Equality>("left-foot",
    //                                                       *robot,
    //                                                       lf_frame_name);
    // leftFootTask->Kp(kp_lf*Vector::Ones(6));
    // leftFootTask->Kd(2.0*leftFootTask->Kp().cwiseSqrt());
    // pinocchio::SE3 lf_ref = robot->position(tsid->data(),
    //                                   robot->model().getJointId(lf_frame_name));
    // tsid->addMotionTask(*leftFootTask, w_lf, 1);

    // TrajectorySample ls(12, 6);
    // SE3ToVector(lf_ref, ls.pos);
    // leftFootTask->setReference(ls);

  }
};

const double StandardRomeoInvDynCtrl::lxp = 0.14;
const double StandardRomeoInvDynCtrl::lxn = 0.077;
const double StandardRomeoInvDynCtrl::lyp = 0.069;
const double StandardRomeoInvDynCtrl::lyn = 0.069;
const double StandardRomeoInvDynCtrl::lz = 0.105;
const double StandardRomeoInvDynCtrl::mu = 0.3;
const double StandardRomeoInvDynCtrl::fMin = 5.0;
const double StandardRomeoInvDynCtrl::fMax = 1000.0;
const std::string StandardRomeoInvDynCtrl::rf_frame_name = "RAnkleRoll";
const std::string StandardRomeoInvDynCtrl::lf_frame_name = "LAnkleRoll";
const Vector3 StandardRomeoInvDynCtrl::contactNormal = Vector3::UnitZ();
const double StandardRomeoInvDynCtrl::w_com = 1.0;
const double StandardRomeoInvDynCtrl::w_lf = 1e3;
const double StandardRomeoInvDynCtrl::w_rf = 1e3;
const double StandardRomeoInvDynCtrl::w_posture = 1e-2;
const double StandardRomeoInvDynCtrl::w_forceReg = 1e-5;
const double StandardRomeoInvDynCtrl::kp_contact = 100.0;
const double StandardRomeoInvDynCtrl::kp_com = 30.0;
const double StandardRomeoInvDynCtrl::kp_lf = 100.0;
const double StandardRomeoInvDynCtrl::kp_rf = 100.0;
const double StandardRomeoInvDynCtrl::kp_posture = 30.0;



BOOST_AUTO_TEST_CASE ( test_invdyn_formulation_acc_force )
{
  cout<<"\n*** test_invdyn_formulation_acc_force ***\n";

  const double dt = 0.001;
  const unsigned int PRINT_N = 1000;
  double t = 0.0;

  StandardRomeoInvDynCtrl romeo_inv_dyn(dt);
  RobotWrapper & robot = *(romeo_inv_dyn.robot);
  auto tsid = romeo_inv_dyn.tsid;
  Contact6d & contactRF = *(romeo_inv_dyn.contactRF);
  Contact6d & contactLF = *(romeo_inv_dyn.contactLF);
  TaskComEquality & comTask = *(romeo_inv_dyn.comTask);
  TaskJointPosture & postureTask = *(romeo_inv_dyn.postureTask);
  Vector q = romeo_inv_dyn.q;
  Vector v = romeo_inv_dyn.v;
  const int nv = robot.model().nv;

  Vector3 com_ref = robot.com(tsid->data());
  com_ref(1) += 0.1;
  auto trajCom = std::make_shared<TrajectoryEuclidianConstant>("traj_com", com_ref);
  TrajectorySample sampleCom(3);

  Vector q_ref = q.tail(nv-6);
  auto trajPosture = std::make_shared<TrajectoryEuclidianConstant>("traj_posture", q_ref);
  TrajectorySample samplePosture(nv-6);

  // Create an HQP solver
  SolverHQPBase * solver = SolverHQPFactory::createNewSolver(SOLVER_HQP_EIQUADPROG,
                                                               "solver-eiquadprog");

  solver->resize(tsid->nVar(), tsid->nEq(), tsid->nIn());
  cout<<"nVar "<<tsid->nVar()<<endl;
  cout<<"nEq "<<tsid->nEq()<<endl;
  cout<<"nIn "<<tsid->nIn()<<endl;
  cout<<"Initial CoM position: "<<robot.com(tsid->data()).transpose()<<endl;
  cout<<"Initial RF position: "<<romeo_inv_dyn.H_rf_ref<<endl;
  cout<<"Initial LF position: "<<romeo_inv_dyn.H_lf_ref<<endl;

  Vector dv = Vector::Zero(nv);

  for(int i=0; i<max_it; i++)
  {
    sampleCom = trajCom->computeNext();
    comTask.setReference(sampleCom);
    samplePosture = trajPosture->computeNext();
    postureTask.setReference(samplePosture);

    const HQPData & HQPData = tsid->computeProblemData(t, q, v);
    if(i==0)
      cout<< HQPDataToString(HQPData, false)<<endl;

    const HQPOutput & sol = solver->solve(HQPData);

    BOOST_CHECK_MESSAGE(sol.status==HQP_STATUS_OPTIMAL, "Status "+toString(sol.status));
    dv = tsid->getAccelerations(sol);
    v += dt*dv;
    q = pinocchio::integrate(robot.model(), q, dt*v);
    t += dt;

    if(i%PRINT_N==0)
      cout<<"CoM position: "<<robot.com(tsid->data()).transpose()<<endl;
    
    static std::ofstream ofs_q("q.dat");
    ofs_q << q.transpose() << std::endl;

  }

  delete solver;
  cout<<"\n### TEST FINISHED ###\n";
  PRINT_VECTOR(v);
  cout<<"Final   CoM position: "<<robot.com(tsid->data()).transpose()<<endl;
  cout<<"Desired CoM position: "<<com_ref.transpose()<<endl;
}




BOOST_AUTO_TEST_SUITE_END ()

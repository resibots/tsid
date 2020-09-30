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

#include "tsid/math/utils.hpp"
#include "tsid/tasks/task-manipulability-equality.hpp"
#include "tsid/robots/robot-wrapper.hpp"

namespace tsid
{
  namespace tasks
  {
    using namespace math;
    using namespace trajectories;
    using namespace pinocchio;

    TaskManipEquality::TaskManipEquality(const std::string & name,
                                     RobotWrapper & robot,
                                     const std::string & frameName):
      TaskMotion(name, robot),
      m_frame_name(frameName),
      m_constraint(name, 3, robot.nv())
    {
      std::cout <<  " brgin init" << std::endl;

      assert(m_robot.model().existJointName(frameName));
      m_joint_id = m_robot.model().getJointId(frameName);

    
      m_M.setZero();
      m_M_ref.setZero();
      m_J.resize(6, robot.nv());
      m_J.setZero();
      // Eigen::array<long int, 3> dims = {6, robot.nv(), robot.nv()};
      // m_H.resize({6, robot.nv(), robot.nv()});
      // m_H.setZero();
      std::cout <<  " end init" << std::endl;
      // m_mask.resize(6);
      // m_mask.fill(1.);
      // setMask(m_mask);
    }

    void TaskManipEquality::setMask(math::ConstRefVector mask)
    {
      // TaskMotion::setMask(mask);
      // int n = dim();
      // m_constraint.resize(n, (unsigned int)m_J.cols());
      // m_p_error_masked_vec.resize(n);
      // m_v_error_masked_vec.resize(n);
      // m_drift_masked.resize(n);
      // m_a_des_masked.resize(n);
    }


    void TaskManipEquality::Kp(ConstRefVector Kp)
    {
      assert(Kp.size()==6);
      m_Kp = Kp;
    }

    void TaskManipEquality::Kd(ConstRefVector Kd)
    {
      assert(Kd.size()==6);
      m_Kd = Kd;
    }


    // const Vector3 & TaskComEquality::Kp(){ return m_Kp; }

    // const Vector3 & TaskComEquality::Kd(){ return m_Kd; }

    // void TaskComEquality::Kp(ConstRefVector Kp)
    // {
    //   assert(Kp.size()==3);
    //   m_Kp = Kp;
    // }

    // void TaskComEquality::Kd(ConstRefVector Kd)
    // {
    //   assert(Kd.size()==3);
    //   m_Kd = Kd;
    // }

    int TaskManipEquality::dim() const
    {
      return 3;
    }

    void TaskManipEquality::setReference(const Matrix6 & M, const Data::Tensor3x & M_J)
    {
      m_M_ref = M;
    }

    Index TaskManipEquality::frame_id() const
    {
      return m_joint_id;
    }

    const ConstraintBase & TaskManipEquality::getConstraint() const
    {
      return m_constraint;
    }


    Eigen::MatrixXd  TaskManipEquality::unfold(int mode, Data::Tensor3x  T)
    {
      // Data::Tensor3x test(4,3,2);
      // test.setRandom();
      // for(int i = 0; i<4; i ++){
      //   for(int j = 0; j<3; j++){      
      //     for(int k = 0; k<2; k++){
      //       std::cout <<i+1<<j+1<<k+1<<" : " <<test(i,j,k)<< std::endl;
      //     }
      //   }
      // }
    // std::cout << "mat" << std::endl;
    Eigen::VectorXd toto = Eigen::Map<Eigen::VectorXd>(T.data(),T.size(),1);   
    if(mode == 1)
      return Eigen::Map<Eigen::MatrixXd>(T.data(), T.dimension(0), T.dimension(1)*T.dimension(2)); //(1)
    if(mode == 2)
      return Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> >(T.data(), T.dimension(2), T.dimension(0)*T.dimension(1)); //(2)
    if(mode == 3){
      Eigen::MatrixXd unfold2(T.dimension(1),T.dimension(0)*T.dimension(2));
      Eigen::MatrixXd tmp = Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> >(T.data(),  T.dimension(1)*T.dimension(2),T.dimension(0));
      unfold2.block(0,0,T.dimension(1),T.dimension(0)) = tmp.block(0,0,T.dimension(1),T.dimension(0));
      unfold2.block(0,T.dimension(0),T.dimension(1),T.dimension(0)) = tmp.block(T.dimension(1),0,T.dimension(1),T.dimension(0));
      return unfold2;
    }
    else
    {
      std::cerr << "Mode should be 1, 2 or 3" << std::endl;
    }

    //    for(int h = 0; h< unfold2.rows(); h++){
    //   for(int g = 0; g< unfold2.cols(); g++){
    //     for(int i = 0; i<4; i ++){
    //       for(int j = 0; j<3; j++){      
    //         for(int k = 0; k<2; k++){
    //           if(unfold2(h,g)==test(i,j,k))
    //             std::cout <<i+1<<j+1<<k+1<<" : "<< unfold2(h,g) << std::endl;
    //         }
    //       }
    //     }
    //   }
    // }
    // std::cout << "unfold2" << unfold2 << std::endl;
    }

    void TaskManipEquality::fold(int mode,const Eigen::MatrixXd & M, Data::Tensor3x & folded)
    {
      if(mode==1){
        double *data_folded = folded.data();
        for(long int k = 0; k < folded.dimension(2); k++){
          for(long int i = 0; i < folded.dimension(0); i++){
            for(long int j = 0; j < folded.dimension(1); j++){
              data_folded[i + j*folded.dimension(0) + k*folded.dimension(0)*folded.dimension(1)] = m_J(i+k*folded.dimension(1),j);
            }
          }
        }
      }

      if(mode==2){
        double *data_folded = folded.data();
        for(long int k = 0; k < folded.dimension(2); k++){
          for(long int i = 0; i < folded.dimension(0); i++){
            for(long int j = 0; j < folded.dimension(1); j++){
              data_folded[i + j*folded.dimension(0) + k*folded.dimension(0)*folded.dimension(1)] = m_J(j+k*folded.dimension(1),i);
            }
          }
        }
      }
    }

    
    const ConstraintBase & TaskManipEquality::compute(const double t,
                                                    ConstRefVector q,
                                                    ConstRefVector v,
                                                    Data & data)
    {
      std::cout <<  " start compute " << m_joint_id << std::endl;

      // pinocchio::Data data_cp = data;
      // m_robot.jacobianWorld(data, m_joint_id, m_J);
      //  m_robot.frameJacobianWorld(data, m_joint_id, m_J);
      getJointJacobian(m_robot.model(), data, m_joint_id, pinocchio::WORLD, m_J);
      std::cout <<  " start compute 2" << std::endl;

      m_M = m_J*m_J.transpose();
      std::cout <<  " start compute 3" << std::endl;

      computeJointKinematicHessians(m_robot.model(), data);
      // std::cout << m_H.dimension(0)<< " "<<  m_H.dimension(1) << " " << m_H.dimension(2) << " " << m_robot.nv() << std::endl;;

      Data::Tensor3x H = getJointKinematicHessian(m_robot.model(), data, m_joint_id, WORLD);
      std::cout << "H " << H.dimension(0)<< " "<<  H.dimension(1) << " " << H.dimension(2) << " " << m_robot.nv() << std::endl;;


      Tensor<double,3,0,long int> H_transpose(H.dimension(1),H.dimension(0),H.dimension(2));
      double *data_transpose = H_transpose.data();
      for(long int i = 0; i < H.dimension(0); i++){
        for(long int j = 0; j < H.dimension(1); j++){
          for(long int k = 0; k < H.dimension(2); k++){
            data_transpose[j + i*H.dimension(1) + k*H.dimension(0)*H.dimension(1)] = H(i,j,k);
          }
        }
      }



      Eigen::MatrixXd H_t_J_mat =  m_J*(unfold(1,H_transpose));
      Tensor<double,3,0,long int> H_t_J(m_J.rows(),H.dimension(1),H.dimension(2));
      Eigen::MatrixXd H_J_mat = m_J*unfold(2,H);
      Tensor<double,3,0,long int> H_J(H.dimension(0),m_J.rows(),H.dimension(2));
      fold(1,H_t_J_mat,H_t_J);
      fold(2,H_J_mat,H_J);


     Eigen::MatrixXd M_J = unfold(3,H_J) + unfold(3,H_t_J);
    //  Eigen::MatrixXd M_J_dot = v.transpose*M_J;
    // for(long int i = 0; i < H.dimension(0); i++){
    //     for(long int j = 0; j < H.dimension(1); j++){
    //       for(long int k = 0; k < H.dimension(2); k++){
    //         std::cout <<"H(i,j,k) " << H(i,j,k)<< " H_transpose(j,i,k) " <<H_transpose(j,i,k) << std::endl;
    //       }
    //     }
    //   }

  
      std::cout <<"m_J " << m_J.rows()<< " "<<  m_J.cols() << std::endl;
      std::cout <<"unfold(2,H) " << unfold(2,H).rows()<< " "<<  unfold(2,H).cols() << std::endl;
      std::cout <<"unfold(1,H_transpose)" << unfold(1,H_transpose).rows()<< " "<<  unfold(1,H_transpose).cols() << std::endl;
      std::cout << "M_J " << M_J.rows()<< " "<<  M_J.cols() << std::endl;




      Matrix3x Jcom = m_robot.Jcom(data);
      Jcom.setZero();
      m_constraint.setMatrix(Jcom);
      Vector3 toto;
      toto.setZero();
      m_constraint.setVector(toto);
      return m_constraint;
    }
  }
}

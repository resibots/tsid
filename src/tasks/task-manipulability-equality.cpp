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
#include <unsupported/Eigen/MatrixFunctions>
namespace tsid
{
  namespace tasks
  {
    using namespace math;
    using namespace trajectories;
    using namespace pinocchio;

    TaskManipEquality::TaskManipEquality(const std::string & name,
                                     RobotWrapper & robot,
                                     const std::string & frameName,
                                     float dt):
      TaskMotion(name, robot),
      m_frame_name(frameName),
      m_constraint(name, 6*6, robot.nv()),
      m_dt(dt)
    {
      std::cout <<  " brgin init" << std::endl;

      assert(m_robot.model().existJointName(frameName));
      m_joint_id = m_robot.model().getJointId(frameName);
    
      m_constraint_prev.resize(robot.nv(),6*6);
      m_constraint_prev.setZero();
      m_M.setZero();
      m_M_ref.setZero();
      m_M_dot_mat.setZero();
      m_M_dot_mat_ref.setZero();
      m_M_dot_dot_mat_ref.setZero();
      m_J.resize(6, robot.nv());
      m_J.setZero();
      Eigen::array<long int,3> dims = {6,6,robot.nv()};
      m_MJ = std::make_shared<pinocchio::Tensor<double,3,0,long int> >(dims);
      m_MJ->setZero();


      dims = {6,robot.nv(),robot.nv()};
      m_H = std::make_shared<pinocchio::Tensor<double,3,0,long int> >(dims);
      m_H->setZero();

      dims = {robot.nv(),6,robot.nv()};
      m_H_transpose = std::make_shared<pinocchio::Tensor<double,3,0,long int> >(dims);
      m_H_transpose->setZero();


      dims = {robot.nv(),6,robot.nv()};
      m_H_transpose = std::make_shared<pinocchio::Tensor<double,3,0,long int> >(dims);
      m_H_transpose->setZero();


      dims = {robot.nv(),6,robot.nv()};
      m_H_transpose = std::make_shared<pinocchio::Tensor<double,3,0,long int> >(dims);
      m_H_transpose->setZero();

      dims = {6, 6, 1};
      m_M_dot = std::make_shared<pinocchio::Tensor<double,3,0,long int> >(dims);
      m_M_dot->setZero();


      m_Kp = 1.0;
      m_Kd = 1.0;


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


    void TaskManipEquality::Kp(float Kp)
    {
      m_Kp = Kp;
    }

    void TaskManipEquality::Kd(float Kd)
    {
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

    void TaskManipEquality::setReference(const Matrix6 & M, const Matrix6 & M_dot, const Matrix6 & M_dot_dot)
    {
      // std::cout << "inside setReference " << std::endl;
      m_M_ref = M;
      m_M_dot_mat_ref = M_dot;
      m_M_dot_dot_mat_ref = M_dot_dot;
    }

    Index TaskManipEquality::frame_id() const
    {
      return m_joint_id;
    }

    const ConstraintEquality & TaskManipEquality::getConstraint() const
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
    if(mode == 3)
      return Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> >(T.data(), T.dimension(2), T.dimension(0)*T.dimension(1)); 
    if(mode == 2){
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
              // std::cout << "i " << i << " j " << j << " k "<< k << std::endl;
              // std::cout << "M " << i << " " << j+k*folded.dimension(1) << std::endl;
              data_folded[i + j*folded.dimension(0) + k*folded.dimension(0)*folded.dimension(1)] = M(i,j+k*folded.dimension(1));
            }
          }
        }
      }

      if(mode==2){
        double *data_folded = folded.data();
        for(long int k = 0; k < folded.dimension(2); k++){
          for(long int i = 0; i < folded.dimension(0); i++){
            for(long int j = 0; j < folded.dimension(1); j++){
              // std::cout << "i " << i << " j " << j << " k "<< k << std::endl;
              // std::cout << "M " << j << " " << i+k*folded.dimension(0) << std::endl;
              data_folded[i + j*folded.dimension(0) + k*folded.dimension(0)*folded.dimension(1)] = M(j,i+k*folded.dimension(0));
            }
          }
        }
      }


      if(mode==3){
        double *data_folded = folded.data();
        for(long int k = 0; k < folded.dimension(2); k++){
          for(long int i = 0; i < folded.dimension(0); i++){
            for(long int j = 0; j < folded.dimension(1); j++){
              // std::cout << "i " << i << " j " << j << " k "<< k << std::endl;
              // std::cout << "M " << k << " " << i+j*folded.dimension(0) << std::endl;
              data_folded[i + j*folded.dimension(0) + k*folded.dimension(0)*folded.dimension(1)] = M(k,i+j*folded.dimension(0));
            }
          }
        }
      }
    }



    Eigen::MatrixXd spdLog(const Eigen::MatrixXd & X,const Eigen::MatrixXd & Y)
    {
      return X.pow(0.5)*X.pow(-0.5)*Y*X.pow(-0.5).log()*X.pow(0.5);
      // return X*((X.inverse()*Y).array().log()).matrix();
    }

    Eigen::VectorXd mandelNotation(const Eigen::MatrixXd & X)
    {

//vectorise with  wrong sizes ?!!!!    
    }

    
    const ConstraintBase & TaskManipEquality::compute(const double t,
                                                    ConstRefVector q,
                                                    ConstRefVector v,
                                                    Data & data)
    {
      // std::cout << "inside compute " << std::endl;
      // std::cout << "q " << q << std::endl;
      // std::cout << "v " << v << std::endl;

      // pinocchio::Data data_cp = data;
      // m_robot.jacobianWorld(data, m_joint_id, m_J);
      //  m_robot.frameJacobianWorld(data, m_joint_id, m_J);

       getJointJacobian(m_robot.model(), data, m_joint_id, pinocchio::WORLD, m_J);

      m_M = m_J*m_J.transpose();

      computeJointKinematicHessians(m_robot.model(), data);
      // std::cout << m_H.dimension(0)<< " "<<  m_H.dimension(1) << " " << m_H.dimension(2) << " " << m_robot.nv() << std::endl;;

      *m_H = getJointKinematicHessian(m_robot.model(), data, m_joint_id, WORLD);
      // std::cout << "m_H " << m_H->dimension(0)<< " "<<  m_H->dimension(1) << " " << m_H->dimension(2) << " " << std::endl;;


      double *data_transpose = m_H_transpose->data();
      for(long int i = 0; i < m_H->dimension(0); i++){
        for(long int j = 0; j < m_H->dimension(1); j++){
          for(long int k = 0; k < m_H->dimension(2); k++){
            data_transpose[j + i*m_H->dimension(1) + k*m_H->dimension(0)*m_H->dimension(1)] = (*m_H)(i,j,k);
          }
        }
      }



    //   // for(long int i = 0; i < m_H->dimension(0); i++){
    //   //   for(long int j = 0; j < m_H->dimension(1); j++){
    //   //     for(long int k = 0; k < m_H->dimension(2); k++){
    //   //       std::cout <<"(*m_H)(i,j,k) " << (*m_H)(i,j,k)<< " m_H_transpose(j,i,k) " <<(*m_H_transpose)(j,i,k) << std::endl;
    //   //     }
    //   //   }
    //   // }



      Eigen::MatrixXd H_t_J_mat =  m_J*(unfold(1,*m_H_transpose));
      Eigen::MatrixXd H_J_mat = m_J*unfold(2,*m_H);


      // std::cout <<"H_J_mat " << H_J_mat.rows()<< " "<<  H_J_mat.cols() << std::endl;
      // std::cout << m_J.rows() << m_H_transpose->dimension(1) << m_H_transpose->dimension(2) << std::endl;
      Tensor<double,3,0,long int> H_t_J(m_J.rows(),m_H_transpose->dimension(1),m_H_transpose->dimension(2));
      fold(1,H_t_J_mat,H_t_J);

      
      
      Tensor<double,3,0,long int> H_J(m_H->dimension(0),m_J.rows(),m_H->dimension(2));

      fold(2,H_J_mat,H_J);


     Eigen::MatrixXd MJ_mat = unfold(3,H_J) + unfold(3,H_t_J);
     fold(3,MJ_mat,*m_MJ);

    
    auto constraint_matrix = unfold(3,*m_MJ);
    
    Eigen::MatrixXd constraint_matrix_dot = (constraint_matrix-m_constraint_prev)/m_dt;
     if(m_constraint_prev.isZero(0)){
        constraint_matrix_dot.setZero();
     }
      // std::cout << "m_constraint_prev " << m_constraint_prev <<  std::endl;
      to_compare_.resize(m_constraint_prev.rows(), m_constraint_prev.cols());
      to_compare_ = m_constraint_prev;

     
      // if(init){
      //   constraint_matrix_dot.setZero();
      //   init = false;
      //   }


      m_constraint_prev=constraint_matrix;
      Eigen::MatrixXd M_dot = v.transpose()*unfold(3,*m_MJ);
      fold(3,M_dot,*m_M_dot); 
      m_M_dot_mat = unfold(1, *m_M_dot);



   


      // auto M_dot_dot  = m_Kp*spdLog(m_M,m_M_ref) + m_Kd*(m_M_dot_mat - m_M_dot_mat_ref);
    //  auto M_dot_dot  = -m_Kp*spdLog(m_M,m_M_ref) - m_Kd*(m_M_dot_mat - m_M_dot_mat_ref) + m_M_dot_dot_mat_ref;
     auto M_dot_dot  = -m_Kp*(m_M - m_M_ref) - m_Kd*(m_M_dot_mat - m_M_dot_mat_ref) + m_M_dot_dot_mat_ref;
    // auto M_dot_dot  = -m_Kp*spdLog(m_M,m_M_ref) - m_Kd*(m_M_dot_mat - m_M_dot_mat_ref);// + m_M_dot_dot_mat_ref;

    //  std::cout << "m_M \n" << m_M << std::endl;
    //  std::cout << "(m_M_ref) \n" << m_M_ref << std::endl;
     
    //  std::cout << "spdLog(m_M,m_M_ref) \n" << spdLog(m_M,m_M_ref) << std::endl;
    //  std::cout << "(m_M_dot_mat - m_M_dot_mat_ref)  \n" << (m_M_dot_mat - m_M_dot_mat_ref)  << std::endl;
    //  std::cout << "m_M_dot_mat  \n" << m_M_dot_mat << std::endl;
    //  std::cout << "m_M_dot_mat_ref  \n" << m_M_dot_mat_ref << std::endl;
    //  std::cout << "m_M_dot_dot_mat_ref  \n" << m_M_dot_dot_mat_ref  << std::endl;
    //   // auto M_dot_dot = m_M_dot_mat_ref - m_M_dot_mat / dt;

      // auto M_dot_dot  = m_Kp*(m_M - m_M_ref) + m_Kd*(m_M_dot_mat - m_M_dot_mat_ref);
      // std::cout << "m_M " <<m_M << std::endl;
      // std::cout << "m_M_ref " << m_M_ref << std::endl;
      // std::cout <<"m_Kp " << m_Kp << " m_Kd " << m_Kd << std::endl;
      // std::cout << "M_dot_dot " <<M_dot_dot << std::endl;
      // // std::cout << "m_M-m_M_ref " <<m_M-m_M_ref << std::endl;
      // // std::cout << "m_M_dot_mat-m_M_dot_mat_ref " <<m_M-m_M_ref << std::endl;
      // std::cout << "M_dot_dot.rows() " <<M_dot_dot.rows() << std::endl;
      // std::cout << "M_dot_dot.cols() " <<M_dot_dot.cols() << std::endl;

      Tensor<double,3,0,long int> M_dot_dot_T(M_dot_dot.rows(),M_dot_dot.cols(),1);
      fold(1,M_dot_dot, M_dot_dot_T);
      auto a = constraint_matrix.transpose();
      Eigen::MatrixXd b  = unfold(3,M_dot_dot_T).transpose() - constraint_matrix_dot.transpose()*v;
      Eigen::VectorXd b_vec = Eigen::Map<Eigen::VectorXd>(b.data(), b.rows(), b.cols());



      // to_compare_.resize(a.rows(), a.cols());
      // to_compare_ = a;

     
     



    //  auto a  = -constraint_matrix.transpose();
    //  Eigen::MatrixXd b  =   constraint_matrix_dot.transpose()*v;
    //  Eigen::VectorXd b_vec = Eigen::Map<Eigen::VectorXd>(b.data(), b.rows(), b.cols());
  
      // std::cout << "M_dot_dot "<< M_dot_dot.rows() << " " << M_dot_dot.cols() << std::endl; 
      // std::cout <<"m_J " << m_J.rows()<< " "<<  m_J.cols() << std::endl;
      // std::cout <<"unfold(2,H) " << unfold(2,*m_H).rows()<< " "<<  unfold(2,*m_H).cols() << std::endl;
      // std::cout <<"unfold(1,H_transpose)" << unfold(1,*m_H_transpose).rows()<< " "<<  unfold(1,*m_H_transpose).cols() << std::endl;
      // std::cout << "unfold(3,H_J) " << unfold(3,H_J).rows()<< " "<<  unfold(3,H_J).cols() << std::endl;
      // std::cout << "MJ_mat " << MJ_mat.rows()<< " "<<  MJ_mat.cols() << std::endl;
      // std::cout <<"constraint_matrix " << constraint_matrix.rows()<< " "<<  constraint_matrix.cols() << std::endl;
      // std::cout << "M_dot " << M_dot.rows()<< " "<<  M_dot.cols() << std::endl;
      // std::cout << "m_M_dot_mat " << m_M_dot_mat.rows()<< " "<<  m_M_dot_mat.cols() << std::endl;
      // std::cout << "a " << a.rows()<< " "<<  a.cols() << std::endl;
      // std::cout << "b " << b.rows()<< " "<<  b.cols() << std::endl;



      Matrix3x Jcom = m_robot.Jcom(data);
      Jcom.setZero();
      m_constraint.setMatrix(a);
      Vector3 toto;
      toto.setZero();
      m_constraint.setVector(b_vec);
      return m_constraint;
    }
  }
}

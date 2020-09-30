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

#ifndef __invdyn_task_manipulability_equality_hpp__
#define __invdyn_task_manipulability_equality_hpp__

#include "tsid/tasks/task-motion.hpp"
#include "tsid/trajectories/trajectory-base.hpp"
#include "tsid/math/constraint-equality.hpp"

#include <pinocchio/multibody/model.hpp>
#include <pinocchio/multibody/data.hpp>
#include "pinocchio/algorithm/jacobian.hpp"
#include "pinocchio/algorithm/kinematics-derivatives.hpp"


namespace tsid
{
  namespace tasks
  {

    class TaskManipEquality : public TaskMotion
    {
    public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW

      typedef math::Index Index;
      typedef trajectories::TrajectorySample TrajectorySample;
      typedef math::Vector Vector;
      typedef math::ConstraintEquality ConstraintEquality;
      typedef pinocchio::Data Data;
      typedef pinocchio::Data::Matrix6x Matrix6x;
      typedef pinocchio::Data::Matrix6 Matrix6;
      typedef pinocchio::Motion Motion;
      typedef pinocchio::SE3 SE3;

      TaskManipEquality(const std::string &name,
                        RobotWrapper &robot,
                        const std::string &frameName);

      int dim() const;

      const ConstraintBase &compute(const double t,
                                    ConstRefVector q,
                                    ConstRefVector v,
                                    Data &data);

      const ConstraintBase &getConstraint() const;

      void setReference(const Matrix6 & M, const Data::Tensor3x & M_J);

  
      virtual void setMask(math::ConstRefVector mask);
      void Kp(ConstRefVector Kp);
      void Kd(ConstRefVector Kp);


      Index frame_id() const;

      Eigen::MatrixXd unfold(int mode,  Data::Tensor3x  T);
      void fold(int mode,const Eigen::MatrixXd & M, Data::Tensor3x & folded);

    protected:
      Vector m_Kp;
      Vector m_Kd;
      std::string m_frame_name;
      Index m_joint_id;

     
      
      Matrix6x m_J;//jacobian
      Matrix6 m_M, m_M_ref; //maniuplability
      std::shared_ptr<Data::Tensor3x> m_M_J;
      std::shared_ptr<Data::Tensor3x> m_M_J; //maniuplability jacobian 

      ConstraintEquality m_constraint;
    };

  } // namespace tasks
} // namespace tsid

#endif // ifndef __invdyn_task_se3_equality_hpp__

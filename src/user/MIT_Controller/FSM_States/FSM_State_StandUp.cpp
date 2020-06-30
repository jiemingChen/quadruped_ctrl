/*============================= Stand Up ==============================*/
/**
 * Transitionary state that is called for the robot to stand up into
 * balance control mode.
 */

#include "FSM_State_StandUp.h"

/**
 * Constructor for the FSM State that passes in state specific info to
 * the generic FSM State constructor.
 *
 * @param _controlFSMData holds all of the relevant control data
 */
template <typename T>
FSM_State_StandUp<T>::FSM_State_StandUp(ControlFSMData<T>* _controlFSMData)
    : FSM_State<T>(_controlFSMData, FSM_StateName::STAND_UP, "STAND_UP"),
      _ini_foot_pos(4) {
  // Do nothing
  // Set the pre controls safety checks
  this->checkSafeOrientation = false;

  // Post control safety checks
  this->checkPDesFoot = false;
  this->checkForceFeedForward = false;
}

template <typename T>
void FSM_State_StandUp<T>::onEnter() {
  // Default is to not transition
  this->nextStateName = this->stateName;

  // Reset the transition data
  this->transitionData.zero();

  // Reset iteration counter
  iter = 0;

  for (size_t leg(0); leg < 4; ++leg) {
    // _ini_foot_pos[leg] = this->_data->_legController->datas[leg].p;
  }
  _ini_foot_pos[0] << 0.0, -0.08, -0.05;
  _ini_foot_pos[1] << 0.0, 0.08, -0.05;
  _ini_foot_pos[2] << 0.0, -0.08, -0.05;
  _ini_foot_pos[3] << 0.0, 0.08, -0.05;
     
}

/**
 * Calls the functions to be executed on each control loop iteration.
 */
template <typename T>
void FSM_State_StandUp<T>::run() {
  if (this->_data->_quadruped->_robotType == RobotType::MINI_CHEETAH) {
    T hMax = 0.25;
    T progress = 1.5 * iter * this->_data->controlParameters->controller_dt;

    if (progress > 1.) {
      progress = 1.;
    }

    // this->_data->_legController->commands[0].kpCartesian =
    //     Vec3<T>(300, 500, 450).asDiagonal();
    // this->_data->_legController->commands[0].kdCartesian =
    //     Vec3<T>(10, 8, 8).asDiagonal();
    // this->_data->_legController->commands[1].kpCartesian =
    //     Vec3<T>(300, 450, 450).asDiagonal();
    // this->_data->_legController->commands[1].kdCartesian =
    //     Vec3<T>(10, 8, 8).asDiagonal();
    // this->_data->_legController->commands[2].kpCartesian =
    //     Vec3<T>(300, 250, 200).asDiagonal();
    // this->_data->_legController->commands[2].kdCartesian =
    //     Vec3<T>(10, 8, 8).asDiagonal();
    // this->_data->_legController->commands[3].kpCartesian =
    //     Vec3<T>(300, 300, 300).asDiagonal();
    // this->_data->_legController->commands[3].kdCartesian =
    //     Vec3<T>(10, 8, 8).asDiagonal();

    if (this->_data->controlParameters->running_type == 0) {  // simulation
      for (int i = 0; i < 3; i++) {
        this->_data->_legController->commands[0].kpCartesian(i, i) =
            this->_data->userParameters->sim_kpCartesian_fr[i];
        this->_data->_legController->commands[0].kdCartesian(i, i) =
            this->_data->userParameters->sim_kdCartesian_fr[i];

        this->_data->_legController->commands[1].kpCartesian(i, i) =
            this->_data->userParameters->sim_kpCartesian_fl[i];
        this->_data->_legController->commands[1].kdCartesian(i, i) =
            this->_data->userParameters->sim_kdCartesian_fl[i];

        this->_data->_legController->commands[2].kpCartesian(i, i) =
            this->_data->userParameters->sim_kpCartesian_hr[i];
        this->_data->_legController->commands[2].kdCartesian(i, i) =
            this->_data->userParameters->sim_kdCartesian_hr[i];

        this->_data->_legController->commands[3].kpCartesian(i, i) =
            this->_data->userParameters->sim_kpCartesian_hl[i];
        this->_data->_legController->commands[3].kdCartesian(i, i) =
            this->_data->userParameters->sim_kdCartesian_hl[i];
      }
    } else {  // real robot
      for (int i = 0; i < 3; i++) {
        this->_data->_legController->commands[0].kpCartesian(i, i) =
            this->_data->userParameters->real_kpCartesian_fr[i];
        this->_data->_legController->commands[0].kdCartesian(i, i) =
            this->_data->userParameters->real_kdCartesian_fr[i];

        this->_data->_legController->commands[1].kpCartesian(i, i) =
            this->_data->userParameters->real_kpCartesian_fl[i];
        this->_data->_legController->commands[1].kdCartesian(i, i) =
            this->_data->userParameters->real_kdCartesian_fl[i];

        this->_data->_legController->commands[2].kpCartesian(i, i) =
            this->_data->userParameters->real_kpCartesian_hr[i];
        this->_data->_legController->commands[2].kdCartesian(i, i) =
            this->_data->userParameters->real_kdCartesian_hr[i];

        this->_data->_legController->commands[3].kpCartesian(i, i) =
            this->_data->userParameters->real_kpCartesian_hl[i];
        this->_data->_legController->commands[3].kdCartesian(i, i) =
            this->_data->userParameters->real_kdCartesian_hl[i];
      }
    }

    for (int i = 0; i < 4; i++) {
      this->_data->_legController->commands[i].pDes = _ini_foot_pos[i];
      this->_data->_legController->commands[i].pDes[2] =
          progress * (-hMax) + (1. - progress) * _ini_foot_pos[i][2];
    }
  }
}

/**
 * Manages which states can be transitioned into either by the user
 * commands or state event triggers.
 *
 * @return the enumerated FSM state name to transition into
 */
template <typename T>
FSM_StateName FSM_State_StandUp<T>::checkTransition() {
  this->nextStateName = this->stateName;
  iter++;

  // Switch FSM control mode
  switch ((int)this->_data->controlParameters->control_mode) {
    case K_STAND_UP:
      break;
    case K_BALANCE_STAND:
      this->nextStateName = FSM_StateName::BALANCE_STAND;
      break;

    case K_LOCOMOTION:
      this->nextStateName = FSM_StateName::LOCOMOTION;
      break;

      // case K_VISION:
      //   this->nextStateName = FSM_StateName::VISION;
      //   break;

    case K_PASSIVE:  // normal c
      this->nextStateName = FSM_StateName::PASSIVE;
      break;

    default:
      std::cout << "[CONTROL FSM] Bad Request: Cannot transition from "
                << K_PASSIVE << " to "
                << this->_data->controlParameters->control_mode << std::endl;
  }

  // Get the next state
  return this->nextStateName;
}

/**
 * Handles the actual transition for the robot between states.
 * Returns true when the transition is completed.
 *
 * @return true if transition is complete
 */
template <typename T>
TransitionData<T> FSM_State_StandUp<T>::transition() {
  // Finish Transition
  switch (this->nextStateName) {
    case FSM_StateName::PASSIVE:  // normal
      this->transitionData.done = true;
      break;

    case FSM_StateName::BALANCE_STAND:
      this->transitionData.done = true;
      break;

    case FSM_StateName::LOCOMOTION:
      this->transitionData.done = true;
      break;

      // case FSM_StateName::VISION:
      //   this->transitionData.done = true;
      //   break;

    default:
      std::cout << "[CONTROL FSM] Something went wrong in transition"
                << std::endl;
  }

  // Return the transition data to the FSM
  return this->transitionData;
}

/**
 * Cleans up the state information on exiting the state.
 */
template <typename T>
void FSM_State_StandUp<T>::onExit() {
  // Nothing to clean up when exiting
}

// template class FSM_State_StandUp<double>;
template class FSM_State_StandUp<float>;

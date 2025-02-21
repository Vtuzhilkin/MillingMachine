#include "StepperMotor.h"
#include <cstdlib>

void StepperMotor::SetPorts(GPIO_TypeDef* dir_port, uint16_t dir_pin, GPIO_TypeDef* en_port, uint16_t en_pin, GPIO_TypeDef* end_port, uint16_t end_pin){
	DIR_Port = dir_port;  
	DIR_Pin = dir_pin; 
	EN_Port = en_port; 
	EN_Pin = en_pin;
	END_Port = end_port;
	END_Pin = end_pin;
	HAL_GPIO_WritePin(DIR_Port, DIR_Pin, (GPIO_PinState)direction);
	HAL_GPIO_WritePin(EN_Port, EN_Pin, (GPIO_PinState)enabling);
}

void StepperMotor::SetPWM(TIM_HandleTypeDef* ht, uint32_t ch){
	timer = ht;
	channel_timer = ch;
	timer->Instance->ARR = step_divider*200*thread_pitch/velocity;
	timer->Instance->CCR1 = timer->Instance->ARR/2;
}

void StepperMotor::Start(){
	if(number_steps != 0 || !calibrated){
		moving = true;
		HAL_TIM_PWM_Start_IT(timer, channel_timer);
	}
}

void StepperMotor::Stop(){
	moving = false;
	HAL_TIM_PWM_Stop_IT(timer, channel_timer);
}


void StepperMotor::CalculateNumberSteps(int32_t finish_coordinate){
	direction  = finish_coordinate > current_number_steps ? positive_direction : !positive_direction;
	number_steps = abs(finish_coordinate - current_number_steps);
	HAL_GPIO_WritePin(DIR_Port, DIR_Pin, (GPIO_PinState)direction);
}

void StepperMotor::ChangeVelocity(const float ratio){
	if(ratio != 0){
		timer->Instance->ARR = step_divider*200*thread_pitch/(velocity*ratio);
		timer->Instance->CCR1 = timer->Instance->ARR/2;
	}
}


void StepperMotor::SetVelocity(const float new_velociy){
	timer->Instance->ARR = step_divider*200*thread_pitch/(new_velociy);
	timer->Instance->CCR1 = timer->Instance->ARR/2;
	velocity = new_velociy;
}


void StepperMotor::Move(){
	if(taken_steps < number_steps){
		taken_steps++;
		current_number_steps += (direction == positive_direction) ? 1 : -1;
	}else{
		taken_steps = 0;
		Stop();
	}
}

void StepperMotor::CheckEndCap(){
	if(HAL_GPIO_ReadPin(END_Port, END_Pin)){
		calibrated = true;
		Stop();
	}
}

bool StepperMotor::Calibrated(){
	return calibrated;
}

float StepperMotor::GetStepsOneMM(){
	return number_steps_one_mm;
}

bool StepperMotor::GetMoving(){
	return moving;
}


int32_t StepperMotor::GetCurrentNumberSteps(){
	return current_number_steps;
}


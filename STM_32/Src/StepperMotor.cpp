#include "StepperMotor.h"
#include <cstdlib>

void StepperMotor::SetPorts(GPIO_TypeDef* dir_port, uint16_t dir_pin, GPIO_TypeDef* en_port, uint16_t en_pin){
	DIR_Port = dir_port;  
	DIR_Pin = dir_pin; 
	EN_Port = en_port; 
	EN_Pin = en_pin;
	HAL_GPIO_WritePin(DIR_Port, DIR_Pin, (GPIO_PinState)direction);
	HAL_GPIO_WritePin(EN_Port, EN_Pin, (GPIO_PinState)enabling);
}

void StepperMotor::SetPWM(TIM_HandleTypeDef* ht, uint32_t ch){
	timer = ht;
	channel_timer = ch;
	timer->Instance->ARR = step_divider*200/velocity;
	timer->Instance->CCR1 = timer->Instance->ARR/2;
}

void StepperMotor::Start(){
	if(number_steps != 0){
		moving = true;
		HAL_TIM_PWM_Start_IT(timer, channel_timer);
	}
}

void StepperMotor::Stop(){
	moving = false;
	HAL_TIM_PWM_Stop_IT(timer, channel_timer);
}


void StepperMotor::CalculateNumberSteps(int32_t start_coordinate, int32_t finish_coordinate){
	if (finish_coordinate > start_coordinate){
		direction  = positive_direction;
	}else{
		direction = !positive_direction;
	}
	HAL_GPIO_WritePin(DIR_Port, DIR_Pin, (GPIO_PinState)direction);
	if(finish_coordinate > start_coordinate){
		number_steps = finish_coordinate - start_coordinate;
	} else{
		number_steps = start_coordinate - finish_coordinate;
	}
	
}

void StepperMotor::ChangeVelocity(const float ratio){
	if(ratio != 0){
		uint16_t number = step_divider*200/(velocity*ratio);
		timer->Instance->ARR = number;
		timer->Instance->CCR1 = number/2;
	}
}


void StepperMotor::Move(){
	if(taken_steps < number_steps){
		taken_steps++;
	}else{
		taken_steps = 0;
		Stop();
	}
}

uint32_t StepperMotor::GetStepsOneMM(){
	return number_steps_one_mm;
}

bool StepperMotor::GetMoving(){
	return moving;
}


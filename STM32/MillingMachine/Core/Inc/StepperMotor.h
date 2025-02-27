#pragma once
#include "main.h"
#include <algorithm>

class StepperMotor{
	private:	
		TIM_HandleTypeDef* timer;
		uint32_t channel_timer;
		
		GPIO_TypeDef *DIR_Port, *EN_Port, *END_Port;
		uint16_t DIR_Pin, EN_Pin, END_Pin;
		
		float thread_pitch;
		uint8_t step_divider;
		uint8_t number_steps_turn = 200;
		float number_steps_one_mm;
		
		float velocity;		
		bool direction;
		bool positive_direction;
		int32_t current_number_steps = 0;
	
		bool enabling = false;
		bool moving = false;
		bool calibrated = false;
		
		uint32_t number_steps;
		uint32_t taken_steps;
		
	public:
		StepperMotor(float vel, bool dir, float thr_pitch, uint8_t step_div): 
			velocity(vel), direction(dir), thread_pitch(thr_pitch), step_divider(step_div), positive_direction(!direction){
			number_steps_one_mm = float(number_steps_turn*step_divider)/thread_pitch;
		};
		
		void SetPorts(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t);
		void SetPWM(TIM_HandleTypeDef*, uint32_t ch);
		
		void Start();
		void Stop();
			
		void CalculateNumberSteps(int32_t);
		void ChangeVelocity(const float ratio);
		void SetVelocity(const float new_velociy);
		
		void Move();
			
		float GetStepsOneMM();
		bool GetMoving();
		
		int32_t GetCurrentNumberSteps();
		
		bool Calibrated();
		void CheckEndCap();
		
};
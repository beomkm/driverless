

#include "PID.hpp"

PID::PID()
{
}

PID::~PID()
{
}

int PID::process(float input)
{
    scale = input;
    if(scale > 40){
        scale = 40;
    }
    else if(scale < -40){
        scale = -40;
    }
    filtering = (1-alpha)*filtering+alpha*scale; //filtering

    data=filtering; //필터링 된 data

    error=data-goal; //present value - 0

    error_total=Kp*error+Ki*error_integral+Kd*(p_error-error)/cycle_time;
    error_integral=error_integral+error*cycle_time;
    p_error = error;

    steering = error_total*15;
    if(steering > 2000){
        steering = 2000;
    }
    else if(steering < -2000){
        steering = -2000;
    }
    return steering; //scale 변화 상수 조절 // out input value
}

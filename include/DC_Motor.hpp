#pragma once
#include <Arduino.h>

// ──────────────────────────────────────────────
//  Minimal PID that outputs a PWM magnitude
// ──────────────────────────────────────────────
class PID_PWM_Controller {
public:
    float Kp, Ki, Kd;

    PID_PWM_Controller(float P, float I, float D)
        : Kp(P), Ki(I), Kd(D), _integral(0), _prevError(0) {}

    // Returns a PWM magnitude clamped to [pwmMin, 255].
    // Caller decides direction from the sign of error.
    float compute(float error, float dt, float integralLimit = 500.0f,
                  int pwmMin = 60) {
        _integral += error * dt;
        _integral  = constrain(_integral, -integralLimit, integralLimit);

        float derivative = (dt > 0) ? (error - _prevError) / dt : 0;
        float output     = Kp * error + Ki * _integral + Kd * derivative;
        _prevError       = error;

        return constrain(abs(output), pwmMin, 255);
    }

    void reset() { _integral = 0; _prevError = 0; }
    void setGains(float P, float I, float D) { Kp=P; Ki=I; Kd=D; }

private:
    float _integral;
    float _prevError;
};


// ──────────────────────────────────────────────
//  DC_Motor — wraps DRV8872 + quadrature encoder
//  + endstop homing + PID position control
// ──────────────────────────────────────────────
class DC_Motor {
public:
    // ── Construction / initialisation ──────────
    DC_Motor(int pwmPin1,      // DRV8872 IN1  (PWM-capable)
             int pwmPin2,      // DRV8872 IN2  (PWM-capable)
             int endstopPin,   // Limit switch (INPUT_PULLUP, LOW = home)
             int encAPin,      // Encoder channel A (interrupt-capable)
             int encBPin,      // Encoder channel B (interrupt-capable)
             float gearRatio   = 30.0f,
             int   cpr         = 64);

    // Call once in setup() — blocks until homed
    void begin();

    // ── Motion API ─────────────────────────────

    // Set a target flap angle (degrees).  PID runs in update().
    void setFlapTarget(float flapDeg);

    // Returns current flap angle derived from encoder
    float getFlapAngle() const;

    // Immediately cut power (coast / High-Z on DRV8872)
    void coast();

    // Active brake (both IN1 & IN2 HIGH → slow decay on DRV8872)
    void brake();

    // Must be called every loop iteration
    // Returns true once the motor has settled within deadband
    bool update();

    // ── Status ─────────────────────────────────
    bool  isHomed()          const { return _homed; }
    bool  hasLeftHome()      const { return _hasLeftHome; }
    float getShaftAngle()    const;   // degrees
    float getShaftTarget()   const { return _shaftTarget; }
    float getShaftError()    const;
    int32_t getRawCount()    const;

    // ── PID gain access ────────────────────────
    PID_PWM_Controller openPID;   // used when opening  (error > 0)
    PID_PWM_Controller closePID;  // used when closing  (error < 0)

    // ── Encoder ISR — must be public for static trampolines ──
    void handleEncoder();

    // In DC_Motor.hpp — add these public methods:
    void setOpenGains (float P, float I, float D) { openPID.setGains(P, I, D); }
    void setCloseGains(float P, float I, float D) { closePID.setGains(P, I, D); }

private:
    // Pin assignments
    int _pin1, _pin2, _endstopPin, _encA, _encB;

    // Motor geometry
    float   _gearRatio;
    int     _cpr;

    // Encoder state (volatile — touched by ISR)
    volatile int32_t _count;
    volatile uint8_t _lastEncoded;

    // Homing state
    bool _homed;
    bool _hasLeftHome;

    // Control state
    float         _shaftTarget;   // degrees
    unsigned long _prevTime;
    const float   _deadband  = 1.0f;   // degrees shaft
    const int     _homingPWM = 60;    // slow creep toward endstop

    // Kinematics helpers (match your original curve-fit polynomials)
    static float shaftAngleFromFlap(float flapDeg);
    static float flapAngleFromShaftError(float shaftErr);

    // Low-level drive
    void _driveForward(int pwm);   // IN1=PWM, IN2=LOW
    void _driveReverse(int pwm);   // IN1=LOW,  IN2=PWM
    void _stop();                  // IN1=LOW,  IN2=LOW  (coast)

    // Singleton pointer so static ISR trampolines can reach the instance
    static DC_Motor* _instance;
    static void _isrA();
    static void _isrB();
};
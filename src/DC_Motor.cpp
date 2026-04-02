#include "DC_Motor.hpp"

// ── Static singleton pointer ───────────────────────────────────────────────
DC_Motor* DC_Motor::_instance = nullptr;

// ── Constructor ───────────────────────────────────────────────────────────
DC_Motor::DC_Motor(int pwmPin1, int pwmPin2, int endstopPin,
                   int encAPin, int encBPin,
                   float gearRatio, int cpr)
    : openPID (0.75f, 0.10f, 0.05f),   // same defaults as your sketch
      closePID(0.50f, 0.10f, 0.05f),
      _pin1(pwmPin1), _pin2(pwmPin2),
      _endstopPin(endstopPin),
      _encA(encAPin), _encB(encBPin),
      _gearRatio(gearRatio), _cpr(cpr),
      _count(0), _lastEncoded(0),
      _homed(false), _hasLeftHome(false),
      _shaftTarget(0.0f), _prevTime(0)
{
    _instance = this;  // store for ISR trampolines
}

// ── begin() — pin setup + blocking home sequence ──────────────────────────
void DC_Motor::begin() {
    pinMode(_pin1,       OUTPUT);
    pinMode(_pin2,       OUTPUT);
    pinMode(_endstopPin, INPUT_PULLUP);
    pinMode(_encA,       INPUT_PULLUP);
    pinMode(_encB,       INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(_encA), _isrA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_encB), _isrB, CHANGE);

    // ── Blocking home: creep in reverse until endstop triggers ──
    while (!_homed) {
        if (digitalRead(_endstopPin) == LOW) {
            // Endstop hit — zero the encoder and mark homed
            _stop();
            noInterrupts();
            _count       = 0;
            _lastEncoded = 0;
            interrupts();
            _homed        = true;
            _hasLeftHome  = false;
            _prevTime     = millis();
        } else {
            // Creep toward home (reverse direction)
            _driveReverse(_homingPWM);
        }
    }
}

// ── setFlapTarget ─────────────────────────────────────────────────────────
void DC_Motor::setFlapTarget(float flapDeg) {
    _shaftTarget = shaftAngleFromFlap(flapDeg);
}

// ── getFlapAngle ──────────────────────────────────────────────────────────
float DC_Motor::getFlapAngle() const {
    // Invert the shaft→flap mapping via the error polynomial at zero offset
    // (matches your original getFlapAngle(error) evaluated at shaft position)
    return flapAngleFromShaftError(getShaftAngle());
}

// ── getShaftAngle ─────────────────────────────────────────────────────────
float DC_Motor::getShaftAngle() const {
    return (getRawCount() * 360.0f) / ((float)_cpr * _gearRatio);
}

// ── getShaftError ─────────────────────────────────────────────────────────
float DC_Motor::getShaftError() const {
    return _shaftTarget - getShaftAngle();
}

// ── getRawCount ───────────────────────────────────────────────────────────
int32_t DC_Motor::getRawCount() const {
    noInterrupts();
    int32_t c = _count;
    interrupts();
    return c;
}

// ── update() — call every loop() iteration ────────────────────────────────
//   Returns true when settled inside the deadband.
bool DC_Motor::update() {
    unsigned long now = millis();
    float dt = (now - _prevTime) / 1000.0f;
    if (dt <= 0) dt = 0.001f;
    _prevTime = now;

    // Track whether we've ever left the endstop
    if (digitalRead(_endstopPin) == HIGH) {
        _hasLeftHome = true;
    }

    float error = getShaftError();

    // ── Within deadband: hold position, reset integrators ────────────────
    if (abs(error) < _deadband) {
        _stop();
        openPID.reset();
        closePID.reset();
        return true;   // settled
    }

    // ── Drive toward target ───────────────────────────────────────────────
    if (error > 0) {
        // Need to move forward (open airbrakes)
        int pwm = (int)openPID.compute(error, dt);
        _driveForward(pwm);
    } else {
        // Need to move in reverse (close airbrakes)
        int pwm = (int)closePID.compute(error, dt);
        _driveReverse(pwm);
    }

    return false;  // still moving
}

// ── coast / brake ─────────────────────────────────────────────────────────
void DC_Motor::coast() { _stop(); }

void DC_Motor::brake() {
    // DRV8872: IN1=HIGH, IN2=HIGH → both low-side FETs on = slow decay / brake
    digitalWrite(_pin1, HIGH);
    digitalWrite(_pin2, HIGH);
}

// ── Kinematics (curve-fit polynomials from your original sketch) ──────────
float DC_Motor::shaftAngleFromFlap(float flapDeg) {
    return 0.0038711505f * powf(flapDeg, 3)
         + 0.0278391178f * powf(flapDeg, 2)
         + 44.3527525437f * flapDeg;
}

float DC_Motor::flapAngleFromShaftError(float err) {
    return -0.0000000001f * powf(err, 3)
           - 0.0000014601f * powf(err, 2)
           + 0.0229608765f * err;
}

// ── Low-level drive helpers ───────────────────────────────────────────────

// Forward: IN1 = PWM, IN2 = LOW  →  current OUT1→OUT2
void DC_Motor::_driveForward(int pwm) {
    analogWrite (_pin1, pwm);
    digitalWrite(_pin2, LOW);
}

// Reverse: IN1 = LOW, IN2 = PWM  →  current OUT2→OUT1
void DC_Motor::_driveReverse(int pwm) {
    digitalWrite(_pin1, LOW);
    analogWrite (_pin2, pwm);
}

// Coast: both inputs LOW → High-Z outputs (DRV8872 enters sleep after 1 ms)
void DC_Motor::_stop() {
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, LOW);
}

// ── Encoder ISR ───────────────────────────────────────────────────────────
void DC_Motor::handleEncoder() {
    uint8_t MSB = digitalRead(_encA);
    uint8_t LSB = digitalRead(_encB);

    uint8_t encoded = (MSB << 1) | LSB;
    uint8_t sum     = (_lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 ||
        sum == 0b0010 || sum == 0b1011)  _count++;

    if (sum == 0b1110 || sum == 0b0111 ||
        sum == 0b0001 || sum == 0b1000)  _count--;

    _lastEncoded = encoded;
}

// Static trampolines — Arduino attachInterrupt requires a plain function ptr
void DC_Motor::_isrA() { if (_instance) _instance->handleEncoder(); }
void DC_Motor::_isrB() { if (_instance) _instance->handleEncoder(); }
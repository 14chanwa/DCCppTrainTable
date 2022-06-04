// Loco
class Loco {
  public:
    int locoId; // DCC id for this loco
    int locoStepsNumber;  // 14, 28 or 128
    int locoSpeed;  // Current speed
    bool locoDirectionForward;  // current direction.

    Loco() {};
    Loco(int _locoId);

    void send();
    void stop();
};

Loco::Loco(int _locoId) {
  locoId = _locoId;
  locoStepsNumber = 128;
  locoSpeed = 0;
  locoDirectionForward = true;
};

void Loco::send() {
  DCCpp::setSpeedMain(1, locoId, locoStepsNumber, locoSpeed, locoDirectionForward);
};

void Loco::stop() {
  DCCpp::setSpeedMain(1, locoId, locoStepsNumber, -1, locoDirectionForward); // Force stop
  locoSpeed = 0; // Reset speed to 0
};

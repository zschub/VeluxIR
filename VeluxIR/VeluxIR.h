#ifndef VeluxIR_h
#define VeluxIR_h

class VeluxIR {
public:

  VeluxIR();

  void init(int pin);

  enum command {
    UP,
    DOWN,
    STOP
  };

  void transmit (short motor, enum command direction);

private:
  int pin;

};

#endif

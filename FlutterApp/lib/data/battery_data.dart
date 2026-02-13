
import 'package:flutter/foundation.dart';

class BatteryData extends ChangeNotifier {

  int _soc = 0; // State of Charge in percentage
  int _outputState12V = 0; // State of 12V output
  int _outputState230V = 0; // State of 230V output

  int getSoc() {
    return _soc;
  }

  void setSoc(int newSoc) {
    _soc = newSoc;
  }

  int getOutputState12V() {
    return _outputState12V;
  }

  void setOutputState12V(int newState) {
    _outputState12V = newState;
  }

  int getOutputState230V() {
    return _outputState230V;
  }

  void setOutputState230V(int newState) {
    _outputState230V = newState;
  }
}
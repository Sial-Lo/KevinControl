import 'package:flutter_blue_plus/flutter_blue_plus.dart';


class BtConnectionData {

  BluetoothConnectionState _connectionState = BluetoothConnectionState.disconnected; // state of the bluetooth connection

  BluetoothConnectionState getConnectionState() {
    return _connectionState;
  }

  void setConnectionState(BluetoothConnectionState newConnectionState) {
    _connectionState = newConnectionState;
  }
}
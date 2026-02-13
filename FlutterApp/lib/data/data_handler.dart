import 'package:flutter_my_first_app/data/bt_connection_data.dart';
import 'package:flutter_my_first_app/data/battery_data.dart';

class DataHandler {
  final _btConnectionData = BtConnectionData();
  final _batteryData = BatteryData();

  BtConnectionData getBtConnectionData() {
    return _btConnectionData;
  }

  BatteryData getBatteryData() {
    return _batteryData;
  }
}

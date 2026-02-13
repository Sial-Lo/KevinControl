import 'dart:async';
import 'dart:ffi';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:flutter_my_first_app/data/battery_data.dart';
import 'package:flutter_my_first_app/widgets/ios_toggle_button.dart';
import 'package:flutter_my_first_app/widgets/percentage_circle.dart';

class BatteryScreen extends StatefulWidget {
  const BatteryScreen({super.key, required this.device});

  final BluetoothDevice device;

  @override
  State<BatteryScreen> createState() => _BatteryScreenState();
}

class _BatteryScreenState extends State<BatteryScreen> {
  final BatteryData data = BatteryData();
  final String _characteristicBasics = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
  final String _characteristicWrite = "aa6316cb-878e-4572-a94e-fec129349f85";

  late StreamSubscription<List<int>> _notificationSubscriptionBasics;
  late StreamSubscription<List<int>> _notificationSubscriptionWrite;

  int _showDetails = 0;

  int _soc = 0;
  int _power = 0;
  int _output12VState = 0;
  int _output230VState = 0;

  @override
  void initState() {
    super.initState();

    setNotifyToBasics();
    setNotifyToWrite();

    _readBasic();
    _readWrite();
  }

  @override
  void dispose() {
    _notificationSubscriptionBasics.cancel();
    _notificationSubscriptionWrite.cancel();
    super.dispose();
  }

  void toggleDetails() {
    _showDetails = (_showDetails == 0) ? 1 : 0;
  }

  BluetoothCharacteristic getCharacteristic(String characteristicUuid) {
    for (var service in widget.device.servicesList) {
      for (var characteristic in service.characteristics) {
        if (characteristic.uuid.toString() == characteristicUuid) {
          return characteristic;
        }
      }
    }
    return null!;
  }

  void setNotifyToBasics() async {
    _notificationSubscriptionBasics = getCharacteristic(_characteristicBasics)
        .lastValueStream
        .listen((value) {
          setState(() {
            debugPrint("Notification value: $value");
            if (value.isNotEmpty) {
              _soc = value[0];
              _power = ((value[1] << 8) | (value[2])).toSigned(16);
            }
          });
        });
    await getCharacteristic(_characteristicBasics).setNotifyValue(true);
  }

  void _readBasic() async {
    Future<List<int>> futureValue = getCharacteristic(_characteristicWrite).read();
    List<int> value = await futureValue;
    if (value.isNotEmpty) {
      setState(() {
        _soc = value[0];
        _power = ((value[1] << 8) | (value[2])).toSigned(16);
      });
    }
  }

  void setNotifyToWrite() async {
    _notificationSubscriptionWrite = getCharacteristic(_characteristicWrite)
        .lastValueStream
        .listen((value) {
          setState(() {
            debugPrint("Notification value: $value");
            if (value.isNotEmpty) {
              _output12VState = value[0];
              _output230VState = value[1];
            }
          });
        });
    await getCharacteristic(_characteristicWrite).setNotifyValue(true);
  }

  void _readWrite() async {
    Future<List<int>> futureValue = getCharacteristic(_characteristicWrite).read();
    List<int> value = await futureValue;
    if (value.isNotEmpty) {
      setState(() {
        _output12VState = value[0];
        _output230VState = value[1];
      });
    }
  }

  void writeCharacteristic() async {
    List<int> value = [_output12VState, _output230VState];
    getCharacteristic(_characteristicWrite).write(value);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: Center(
        child: Column(
          children: [
            const SizedBox(height: 20),
            BatteryPercentage(value: _soc),
            const SizedBox(height: 20),
            DefaultTextStyle.merge(
              style: const TextStyle(fontSize: 32, fontWeight: FontWeight.bold),
              child: Text("$_power W"),
            ),
            const SizedBox(height: 20),
            Row(
              mainAxisSize: MainAxisSize.max,
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                IoSToggleButton(
                  value: _output12VState,
                  onChanged: (v) async {
                    _output12VState = v;
                    writeCharacteristic();
                  },
                  title: '12 V',
                  width: 65,
                  height: 40,
                  requireConfirmation: true,
                  confirmDialogTitle: 'Confirm',
                  confirmDialogContentOn: 'Do you want to enable 12 V output?',
                  confirmDialogContentOff:
                      'Do you want to disable 12 V output?',
                ),
                IoSToggleButton(
                  value: _output230VState,
                  onChanged: (v) async {
                    _output230VState = v;
                    writeCharacteristic();
                  },
                  title: '230 V',
                  width: 65,
                  height: 40,
                  requireConfirmation: true,
                  confirmDialogTitle: 'Confirm',
                  confirmDialogContentOn: 'Do you want to enable 230 V output?',
                  confirmDialogContentOff:
                      'Do you want to disable 230 V output?',
                ),
              ],
            ),
            // const SizedBox(height: 24),
            // FloatingActionButton(
            //   onPressed: toggleDetails,
            //   tooltip: 'Show Details',
            //   child: const Icon(Icons.arrow_downward_rounded),
            // ),
            // const SizedBox(height: 24),
            // BatteryDetailed(device: widget.device, showDetails: _showDetails),
          ],
        ),
      ),
    );
  }
}

class BatteryPercentage extends StatelessWidget {
  final int value;
  const BatteryPercentage({super.key, required this.value});

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        PercentageCircle(
          percentage: value.toDouble(),
          size: 160,
          strokeWidth: 12,
          backgroundColor: Colors.grey.shade200,
          title: 'SOC',
        ),
      ],
    );
  }
}

class BatteryDetailed extends StatefulWidget {
  const BatteryDetailed({
    super.key,
    required this.device,
    required this.showDetails,
  });

  final BluetoothDevice device;
  final int showDetails;

  @override
  State<BatteryDetailed> createState() => _BatteryDetailedState();
}

class _BatteryDetailedState extends State<BatteryDetailed> {
  final String _characteristicDetails = "d7be7b90-2423-4d6e-926d-239bc96bb2fd";
  late StreamSubscription<List<int>> _notificationSubscriptionDetails;

  int _batteryCurrent = 0;

  BluetoothCharacteristic _getCharacteristic(String characteristicUuid) {
    for (var service in widget.device.servicesList) {
      for (var characteristic in service.characteristics) {
        if (characteristic.uuid.toString() == characteristicUuid) {
          return characteristic;
        }
      }
    }
    return null!;
  }

  void _setNotifyToDetails() async {
    _notificationSubscriptionDetails =
        _getCharacteristic(_characteristicDetails).lastValueStream.listen((
          value,
        ) {
          setState(() {
            debugPrint("Notification value: $value");
            if (value.isNotEmpty) {
              _batteryCurrent = (value[0] << 8) & (value[1]);
            }
          });
        });
    await _getCharacteristic(_characteristicDetails).setNotifyValue(true);
  }

  @override
  void initState() {
    _setNotifyToDetails();
    super.initState();
  }

  @override
  void dispose() {
    _notificationSubscriptionDetails.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    if (0 == widget.showDetails) {
      _notificationSubscriptionDetails.cancel();
      return SizedBox(height: 24);
    } else {
      _setNotifyToDetails();
      return Row(
        mainAxisSize: MainAxisSize.max,
        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        children: [Text("Hi $_batteryCurrent")],
      );
    }
  }
}

import 'dart:async';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:flutter_my_first_app/widgets/percentage_circle.dart';

class BatteryScreen extends StatefulWidget {
  const BatteryScreen({super.key, required this.device});

  final BluetoothDevice device;

  @override
  State<BatteryScreen> createState() => _BatteryScreenState();
}

class _BatteryScreenState extends State<BatteryScreen> {
  final String _characteristicReceive = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
  final String _characteristicTransmit = "d7be7b90-2423-4d6e-926d-239bc96bb2fd";

  late StreamSubscription<List<int>> _notificationSubscriptionReceive;
  late StreamSubscription<List<int>> _notificationSubscriptionTransmit;

  int _soc = 0;
  double _batteryVoltage = 0;
  int _batteryPower = 0;
  int _acInputPower = 0;
  int _acOutputPower = 0;
  int _dcInputPower = 0;
  int _dcOutputPower = 0;
  int _cellTemperature = 0;

  bool _acInputState = false;
  bool _acOutputState = false;
  bool _dcInputState = false;
  bool _dcOutputState = false;

  bool _requestedDcOutputState = false;
  bool _requestedAcOutputState = false;
  bool _requestedDcInputState = false;

  @override
  void initState() {
    super.initState();

    delayFunction();
    setNotifyToReceive();
    setNotifyToTransmit();
  }

  void delayFunction() async {
    await Future.delayed(const Duration(milliseconds: 250));
    _readReceive();
    _readTransmit();
  }

  @override
  void dispose() {
    _notificationSubscriptionReceive.cancel();
    _notificationSubscriptionTransmit.cancel();
    super.dispose();
  }

  BluetoothCharacteristic getCharacteristic(String characteristicUuid) {
    for (var service in widget.device.servicesList) {
      for (var characteristic in service.characteristics) {
        if (characteristic.uuid.toString() == characteristicUuid) {
          return characteristic;
        }
      }
    }
    throw Exception("Characteristic with UUID $characteristicUuid not found");
  }

  double unpackSingleValue(List<int> value, int startBit)
  {
    return (((value[startBit + 3] << 24) | (value[startBit + 2] << 16) | (value[startBit + 1] << 8)  | (value[startBit])) / 65536);
  }

  void unpackReceivedData(List<int> value) {
    setState(() {
      debugPrint("Notification value: $value");
      if (value.length >= (12*4)) {
        _batteryVoltage = double.parse((unpackSingleValue(value, 0*4)).toStringAsFixed(2));
        _acInputPower = unpackSingleValue(value, 1*4).round().toSigned(16);
        _acOutputPower = unpackSingleValue(value, 2*4).round().toSigned(16);
        _dcInputPower = unpackSingleValue(value, 3*4).round().toSigned(16);
        _dcOutputPower = unpackSingleValue(value, 4*4).round().toSigned(16);
        _soc = (unpackSingleValue(value, 5*4) * 100).round();
        _cellTemperature = unpackSingleValue(value, 6*4).round().toSigned(16);
        _batteryPower = unpackSingleValue(value, 7*4).round().toSigned(16);

        _acInputState = unpackSingleValue(value, 8*4) == 5 ? true : false;
        _acOutputState = unpackSingleValue(value, 9*4) == 5 ? true : false;
        _dcInputState = unpackSingleValue(value, 10*4) == 5 ? true : false;
        _dcOutputState = unpackSingleValue(value, 11*4) == 5 ? true : false;
      }
    });
  }

  void unpackTransmitData(List<int> value) {
    setState(() {
      debugPrint("Notification value: $value");
      if (value.length >= 3) {
        _requestedDcOutputState = value[0] == 1;
        _requestedAcOutputState = value[1] == 1;
        _requestedDcInputState = value[2] == 1;
      }
    });
  }

  void setNotifyToReceive() async {
    _notificationSubscriptionReceive = getCharacteristic(_characteristicReceive)
        .lastValueStream
        .listen((value) {
          unpackReceivedData(value);
        });
    await getCharacteristic(_characteristicReceive).setNotifyValue(true);
  }

  void setNotifyToTransmit() async {
    _notificationSubscriptionTransmit =
        getCharacteristic(_characteristicTransmit).lastValueStream.listen((
          value,
        ) {
          unpackTransmitData(value);
        });
    await getCharacteristic(_characteristicTransmit).setNotifyValue(true);
  }

  void _readReceive() async {
    Future<List<int>> futureValue = getCharacteristic(
      _characteristicReceive,
    ).read();
    List<int> value = await futureValue;
    unpackReceivedData(value);
  }

  void _readTransmit() async {
    Future<List<int>> futureValue = getCharacteristic(
      _characteristicTransmit,
    ).read();
    List<int> value = await futureValue;
    unpackTransmitData(value);
  }

  void writeTransmitCharacteristic() async {
    List<int> value = [
      _requestedDcOutputState ? 1 : 0,
      _requestedAcOutputState ? 1 : 0,
      _requestedDcInputState ? 1 : 0,
    ];
    getCharacteristic(_characteristicTransmit).write(value);
  }

  Future<void> _showConfirmationDialog(
    String title,
    String message,
    VoidCallback onConfirm,
  ) async {
    showCupertinoDialog(
      context: context,
      builder: (BuildContext context) => CupertinoAlertDialog(
        title: Text(title),
        content: Text(message),
        actions: <CupertinoDialogAction>[
          CupertinoDialogAction(
            isDestructiveAction: false,
            onPressed: () {
              Navigator.pop(context);
            },
            child: const Text('Cancel'),
          ),
          CupertinoDialogAction(
            isDestructiveAction: true,
            onPressed: () {
              Navigator.pop(context);
              onConfirm();
            },
            child: const Text('Confirm'),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Theme.of(context).colorScheme.surface,
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.max,
          children: [
            const SizedBox(height: 30),
            DefaultTextStyle.merge(
              style: const TextStyle(fontSize: 25, fontWeight: FontWeight.bold),
              child: Text("Battery"),
            ),
            const SizedBox(height: 30),
            Row(
              mainAxisSize: MainAxisSize.max,
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                Column(
                  mainAxisSize: MainAxisSize.max,
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    Column(
                      mainAxisSize: MainAxisSize.max,
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        DefaultTextStyle.merge(
                          style: const TextStyle(
                            fontSize: 15,
                            fontWeight: FontWeight.bold,
                          ),
                          child: Text("DC Input"),
                        ),
                        Row(
                          mainAxisSize: MainAxisSize.max,
                          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                          children: [
                            Column(
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: [
                                SizedBox(height: 0, width: 40),
                                DefaultTextStyle.merge(
                                  style: const TextStyle(fontSize: 10),
                                  child: Text("$_dcInputPower W"),
                                ),
                              ],
                            ),
                            Icon(
                              CupertinoIcons.arrow_right_circle_fill,
                              color: _dcInputState
                                  ? CupertinoColors.activeGreen
                                  : CupertinoColors.systemGrey,
                              size: 40,
                            ),
                          ],
                        ),
                      ],
                    ),
                    SizedBox(height: 20),
                    Column(
                      mainAxisSize: MainAxisSize.max,
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        DefaultTextStyle.merge(
                          style: const TextStyle(
                            fontSize: 15,
                            fontWeight: FontWeight.bold,
                          ),
                          child: Text("DC Output"),
                        ),
                        Row(
                          children: [
                            Column(
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: [
                                SizedBox(height: 0, width: 40),
                                DefaultTextStyle.merge(
                                  style: const TextStyle(fontSize: 10),
                                  child: Text("$_dcOutputPower W"),
                                ),
                              ],
                            ),
                            Icon(
                              CupertinoIcons.arrow_left_right_circle_fill,
                              color: _dcOutputState
                                  ? CupertinoColors.activeGreen
                                  : CupertinoColors.systemGrey,
                              size: 40,
                            ),
                          ],
                        ),
                      ],
                    ),
                  ],
                ),
                const SizedBox(width: 25),
                BatteryPercentage(value: _soc),
                const SizedBox(width: 25),
                Column(
                  mainAxisSize: MainAxisSize.max,
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    Column(
                      mainAxisSize: MainAxisSize.max,
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        DefaultTextStyle.merge(
                          style: const TextStyle(
                            fontSize: 15,
                            fontWeight: FontWeight.bold,
                          ),
                          child: Text("AC Input"),
                        ),
                        Row(
                          mainAxisSize: MainAxisSize.max,
                          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                          children: [
                            Icon(
                              CupertinoIcons.arrow_left_circle_fill,
                              color: _acInputState
                                  ? CupertinoColors.activeGreen
                                  : CupertinoColors.systemGrey,
                              size: 40,
                            ),
                            Column(
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: [
                                const SizedBox(height: 0, width: 40),
                                DefaultTextStyle.merge(
                                  style: const TextStyle(fontSize: 10),
                                  child: Text("$_acInputPower W"),
                                ),
                              ],
                            ),
                          ],
                        ),
                      ],
                    ),
                    const SizedBox(height: 20),
                    Column(
                      mainAxisSize: MainAxisSize.max,
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        DefaultTextStyle.merge(
                          style: const TextStyle(
                            fontSize: 15,
                            fontWeight: FontWeight.bold,
                          ),
                          child: Text("AC Output"),
                        ),
                        Row(
                          children: [
                            Icon(
                              CupertinoIcons.arrow_right_circle_fill,
                              color: _acOutputState
                                  ? CupertinoColors.activeGreen
                                  : CupertinoColors.systemGrey,
                              size: 40,
                            ),
                            Column(
                              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                              children: [
                                const SizedBox(height: 0, width: 40),
                                DefaultTextStyle.merge(
                                  style: const TextStyle(fontSize: 10),
                                  child: Text("$_acOutputPower W"),
                                ),
                              ],
                            ),
                          ],
                        ),
                      ],
                    ),
                  ],
                ),
              ],
            ),
            const SizedBox(height: 30),
            DefaultTextStyle.merge(
              style: const TextStyle(fontSize: 28, fontWeight: FontWeight.bold),
              child: Text("$_batteryPower W"),
            ),
            const SizedBox(height: 30),
            DefaultTextStyle.merge(
              style: const TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
              child: Text("$_batteryVoltage V"),
            ),
            const SizedBox(height: 10),
            DefaultTextStyle.merge(
              style: const TextStyle(fontSize: 16),
              child: Text("$_cellTemperature °C"),
            ),
            const SizedBox(height: 30),
            Row(
              mainAxisSize: MainAxisSize.max,
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                Column(
                  children: [
                    Text("DC Input"),
                    CupertinoSwitch(
                      value: _requestedDcInputState,
                      activeTrackColor: CupertinoColors.activeGreen,
                      onChanged: (bool value) {
                        _showConfirmationDialog(
                          'Turn ${value ? 'On' : 'Off'} DC Input?',
                          'Are you sure you want to ${value ? 'turn on' : 'turn off'} the DC Input?',
                          () {
                            setState(() {
                              _requestedDcInputState = value;
                              writeTransmitCharacteristic();
                            });
                          },
                        );
                      },
                    ),
                  ],
                ),
                const SizedBox(width: 65, height: 39),
              ],
            ),
            const SizedBox(height: 30),
            Row(
              mainAxisSize: MainAxisSize.max,
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                Column(
                  children: [
                    Text("DC Output"),
                    CupertinoSwitch(
                      value: _requestedDcOutputState,
                      activeTrackColor: CupertinoColors.activeGreen,
                      onChanged: (bool value) {
                        _showConfirmationDialog(
                          'Turn ${value ? 'On' : 'Off'} DC Output?',
                          'Are you sure you want to ${value ? 'turn on' : 'turn off'} the DC Output?',
                          () {
                            setState(() {
                              _requestedDcOutputState = value;
                              writeTransmitCharacteristic();
                            });
                          },
                        );
                      },
                    ),
                  ],
                ),
                Column(
                  children: [
                    Text("AC Output"),
                    CupertinoSwitch(
                      value: _requestedAcOutputState,
                      activeTrackColor: CupertinoColors.activeGreen,
                      onChanged: (bool value) {
                        _showConfirmationDialog(
                          'Turn ${value ? 'On' : 'Off'} AC Output?',
                          'Are you sure you want to ${value ? 'turn on' : 'turn off'} the AC Output?',
                          () {
                            setState(() {
                              _requestedAcOutputState = value;
                              writeTransmitCharacteristic();
                            });
                          },
                        );
                      },
                    ),
                  ],
                ),
              ],
            ),
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
          size: 175,
          strokeWidth: 12,
          backgroundColor: Colors.grey.shade200,
        ),
      ],
    );
  }
}

import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:flutter_my_first_app/data/data_handler.dart';
import 'package:flutter_my_first_app/screens/battery_screen.dart';
import 'package:flutter_my_first_app/utils/extra.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key, required this.device});

  final BluetoothDevice device;

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  var _selectedIndex = 0;
  late StreamSubscription<BluetoothConnectionState> _connectionStateSubscription;

  @override
  void initState() {
    super.initState();

    _connectionStateSubscription = widget.device.connectionState.listen((state) async {
      if (state == BluetoothConnectionState.disconnected) {
        closeScreen();
      }
    });
  }

  @override
  void dispose() {
    _connectionStateSubscription.cancel();
    super.dispose();
  }

  void closeScreen() {
    Navigator.of(context).pop();
  }

  @override
  Widget build(BuildContext context) {
    Widget batteryPage = BatteryScreen(device: widget.device);
    Widget batteryPage2 = BatteryScreen(device: widget.device);

    return MaterialApp(
      color: Theme.of(context).colorScheme.primary,
      home: DefaultTabController(
        length: 3,
        child: Scaffold(
          appBar: AppBar(
            bottom:  TabBar(
              tabs: [
                Tab(icon: Icon(Icons.battery_std)),
                Tab(icon: Icon(Icons.air)),
                Tab(icon: Icon(Icons.water)),
              ],
            ),
            title: const Text('Kevin Conrol'),
          ),
          body:  TabBarView(
            children: [
              batteryPage,
              Placeholder(),
              batteryPage2,
            ],
          ),
        ),
      ),
    );
  }

  // @override
  // Widget build(BuildContext context) {
  //   Widget batteryPage = BatteryScreen(
  //     device: widget.device,
  //   );
  //   Widget batteryPage2 = BatteryScreen(
  //     device: widget.device,
  //   );

  //   Widget page;
  //   switch (_selectedIndex) {
  //     case 0:
  //       page = batteryPage;
  //       break;
  //     case 1:
  //       page = Placeholder();
  //       break;
  //     case 2:
  //       page = batteryPage2;
  //       break;
  //     default:
  //       throw UnimplementedError('no widget for $_selectedIndex');
  //   }

  //   return Scaffold(
  //     body: Row(
  //       children: [
  //         SafeArea(
  //           child: NavigationRail(
  //             extended: false,
  //             labelType: NavigationRailLabelType.all,
  //             groupAlignment: -1.0,
  //             backgroundColor: Theme.of(context).colorScheme.primaryContainer,
  //             destinations: [
  //               // NavigationRailDestination(
  //               //   icon: Icon(Icons.bluetooth),
  //               //   label: Text('Connection'),
  //               // ),
  //               NavigationRailDestination(
  //                 icon: Icon(Icons.battery_std),
  //                 label: Text('Battery'),
  //               ),
  //               NavigationRailDestination(
  //                 icon: Icon(Icons.settings_overscan),
  //                 label: Text('Placeholder'),
  //               ),
  //             ],
  //             selectedIndex: _selectedIndex,
  //             onDestinationSelected: (value) {
  //               setState(() {
  //                 _selectedIndex = value;
  //               });
  //             },
  //           ),
  //         ),
  //         Expanded(
  //           child: Container(
  //             color: Theme.of(context).colorScheme.primaryContainer,
  //             child: page,
  //           ),
  //         ),
  //       ],
  //     ),
  //   );
  // }
}

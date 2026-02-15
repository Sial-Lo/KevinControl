import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:flutter_my_first_app/screens/battery_screen.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key, required this.device});

  final BluetoothDevice device;

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late StreamSubscription<BluetoothConnectionState>
  _connectionStateSubscription;

  @override
  void initState() {
    super.initState();

    _connectionStateSubscription = widget.device.connectionState.listen((
      state,
    ) async {
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
      theme: ThemeData(
        colorScheme: ColorScheme(
          brightness: Brightness.light,
          primary: const Color.fromARGB(255, 89, 131, 146),
          onPrimary: const Color.fromARGB(255, 255, 255, 255),
          secondary: const Color.fromARGB(255, 158, 179, 194),
          onSecondary: const Color.fromARGB(255, 58, 58, 58),
          error: const Color.fromARGB(255, 197, 52, 52),
          onError: const Color.fromARGB(255, 255, 255, 255),
          surface: const Color.fromARGB(255, 255, 255, 255),  
          onSurface: const Color.fromARGB(255, 58, 58, 58),
        ),
      ),
      home: DefaultTabController(
        length: 3,
        child: Scaffold(
          appBar: AppBar(
            backgroundColor: Theme.of(context).colorScheme.primary,
            bottom: TabBar(
              labelColor: Theme.of(context).colorScheme.onPrimary,
              unselectedLabelColor: Theme.of(context).colorScheme.onPrimary.withValues(alpha: 0.6),
              indicatorColor: Theme.of(context).colorScheme.onPrimary,
              dividerColor: Theme.of(context).colorScheme.onPrimary.withValues(alpha: 0.3),
              tabs: [
                Tab(icon: Icon(Icons.battery_std)),
                Tab(icon: Icon(Icons.air)),
                Tab(icon: Icon(Icons.water)),
              ],
            ),
            title: DefaultTextStyle.merge(
              style: TextStyle(
                fontSize: 25,
                fontWeight: FontWeight.bold,
                color: Theme.of(context).colorScheme.onPrimary,
              ),
              child: Text("Kevin Control"),
            ),
            centerTitle: true,
          ),
          body: TabBarView(
            children: [batteryPage, Placeholder(), batteryPage2],
          ),
        ),
      ),
    );
  }
}

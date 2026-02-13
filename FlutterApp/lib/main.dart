import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_my_first_app/data/data_handler.dart';
import 'package:flutter_my_first_app/pages/my_test_page.dart';
import 'package:flutter_my_first_app/pages/battery_page.dart';
import 'package:flutter_my_first_app/screens/scan_screen.dart';

import 'package:flutter_my_first_app/screens/connection_screen.dart';

import 'dart:async';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';



void main() {
  FlutterBluePlus.setLogLevel(LogLevel.verbose, color:false);
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {

  final DataHandler _dataHandler = DataHandler();

  late StreamSubscription<BluetoothAdapterState> _adapterStateStateSubscription;

  @override
  void initState() {
    super.initState();
    _adapterStateStateSubscription = FlutterBluePlus.adapterState.listen((state) {
      if (mounted) {
        setState(() {});
      }
    });
  }

  @override
  void dispose() {
    _adapterStateStateSubscription.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: const Color.fromARGB(255, 83, 135, 231)),
      ),
      home: ScanScreen(),
    );
  }
}
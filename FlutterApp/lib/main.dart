import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:flutter_my_first_app/screens/scan_screen.dart';



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
      title: 'Kevin Control',
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
      home: ScanScreen(),
    );
  }
}
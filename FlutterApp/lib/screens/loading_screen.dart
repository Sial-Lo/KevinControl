import 'package:flutter/material.dart';
import 'dart:async';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

import 'package:flutter_my_first_app/screens/home_screen.dart';

class LoadingScreen extends StatefulWidget {
  const LoadingScreen({super.key, required this.device});

  final BluetoothDevice device;

  @override
  State<LoadingScreen> createState() => _LoadingScreenState();
}

class _LoadingScreenState extends State<LoadingScreen> {
  late StreamSubscription<BluetoothConnectionState> _connectionStateSubscription;

  @override
  void initState() {
    super.initState();

    _connectionStateSubscription = widget.device.connectionState.listen((state) async {
      if (state == BluetoothConnectionState.connected) {
        await widget.device.discoverServices();
        openScreen();

        // MaterialPageRoute routeLoading = MaterialPageRoute(
        //   builder: (context) => HomeScreen(device: widget.device),
        //   settings: RouteSettings(name: '/HomeScreen'),
          
        // );

        // await Navigator.push(context,routeLoading);
      }
      if (mounted) {
        setState(() {});
      }
    });
  }

  @override
  void dispose() {
    _connectionStateSubscription.cancel();
    widget.device.disconnect();
    super.dispose();
  }

  void openScreen() {
    Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) => HomeScreen(device: widget.device),
      ),
    );
  }

  void closeScreen() {
    Navigator.of(context).pop();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text('Connecting...', style: TextStyle(fontSize: 24)),
            SizedBox(height: 52),
            CircularProgressIndicator(
              strokeCap: StrokeCap.round,
              constraints: BoxConstraints.tightFor(width: 120, height: 120),
              strokeWidth: 16,
            ),
            SizedBox(height: 52),

            ElevatedButton(
              onPressed: () {closeScreen();},
              style: ElevatedButton.styleFrom(
                backgroundColor: Theme.of(context).colorScheme.onSurface,
                foregroundColor: Colors.white,
              ),
              child: const Text("Cancel"),
            ),
          ],
        ),
      ),
    );
  }
}

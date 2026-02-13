import 'package:flutter/material.dart';
import 'package:flutter_my_first_app/data/battery_data.dart';
import 'package:flutter_my_first_app/widgets/ios_toggle_button.dart';
import 'package:flutter_my_first_app/widgets/percentage_circle.dart';

class BatteryPage extends StatefulWidget {
  const BatteryPage({super.key, required this.data});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final BatteryData data;

  @override
  State<BatteryPage> createState() => _BatteryPageState();
}

class _BatteryPageState extends State<BatteryPage> {
  int _counter = 0;
  late VoidCallback _batteryListener;

  @override
  void initState() {
    super.initState();
    _batteryListener = () {
      if (mounted) setState(() {});
    };
    widget.data.addListener(_batteryListener);
  }

  @override
  void didUpdateWidget(covariant BatteryPage oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.data != widget.data) {
      oldWidget.data.removeListener(_batteryListener);
      widget.data.addListener(_batteryListener);
    }
  }

  @override
  void dispose() {
    widget.data.removeListener(_batteryListener);
    super.dispose();
  }

  void _incrementCounter() {
    setState(() {
      // This call to setState tells the Flutter framework that something has
      // changed in this State, which causes it to rerun the build method below
      // so that the display can reflect the updated values. If we changed
      // _counter without calling setState(), then the build method would not be
      // called again, and so nothing would appear to happen.
      _counter++;
    });
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      backgroundColor: Theme.of(context).colorScheme.inversePrimary,
      appBar: AppBar(
        centerTitle: true,
        // TRY THIS: Try changing the color here to a specific color (to
        // Colors.amber, perhaps?) and trigger a hot reload to see the AppBar
        // change color while the other colors stay the same.
        backgroundColor: Theme.of(context).colorScheme.primary,
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text('Battery'),
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
          // Column is also a layout widget. It takes a list of children and
          // arranges them vertically. By default, it sizes itself to fit its
          // children horizontally, and tries to be as tall as its parent.
          //
          // Column has various properties to control how it sizes itself and
          // how it positions its children. Here we use mainAxisAlignment to
          // center the children vertically; the main axis here is the vertical
          // axis because Columns are vertical (the cross axis would be
          // horizontal).
          //
          // TRY THIS: Invoke "debug painting" (choose the "Toggle Debug Paint"
          // action in the IDE, or press "p" in the console), to see the
          // wireframe for each widget.
          // mainAxisAlignment: .center,
          children: [
            const SizedBox(height: 20),
            BatteryPercentage(value: widget.data.getSoc()),
            const SizedBox(height: 24),
            BatteryOutputs(batteryData: widget.data),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _incrementCounter,
        tooltip: 'Increment',
        child: const Icon(Icons.add),
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

class BatteryOutputs extends StatelessWidget {
  final BatteryData batteryData;
  const BatteryOutputs({super.key, required this.batteryData});

  @override
  Widget build(BuildContext context) {
    const double buttonWidth = 65;
    const double buttonHeight = 40;

    return Row(
      mainAxisSize: MainAxisSize.max,
      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
      children: [
        IoSToggleButton(
          value: batteryData.getOutputState12V(),
          onChanged: (v) {
            batteryData.setOutputState12V(v);
          },
          title: '12 V',
          width: buttonWidth,
          height: buttonHeight,
          requireConfirmation: true,
          confirmDialogTitle: 'Confirm',
          confirmDialogContentOn: 'Do you want to enable 12 V output?',
          confirmDialogContentOff: 'Do you want to disable 12 V output?',
        ),
        IoSToggleButton(
          value: batteryData.getOutputState230V(),
          onChanged: (v) {
            batteryData.setOutputState230V(v);
          },
          title: '230 V',
          width: buttonWidth,
          height: buttonHeight,
          requireConfirmation: true,
          confirmDialogTitle: 'Confirm',
          confirmDialogContentOn: 'Do you want to enable 230 V output?',
          confirmDialogContentOff: 'Do you want to disable 230 V output?',
        ),
      ],
    );
  }
}

import 'package:flutter/material.dart';

class AnimatedBatteryCharging extends StatefulWidget {
  final int percentage;
  final Duration animationDuration;
  final Color batteryColor;
  final Color backgroundColor;
  final Color textColor;
  final bool isCharging;

  const AnimatedBatteryCharging({
    super.key,
    required this.percentage,
    this.animationDuration = const Duration(milliseconds: 1000),
    this.batteryColor = Colors.green,
    this.backgroundColor = Colors.grey,
    this.textColor = Colors.black,
    this.isCharging = false,
  });

  @override
  State<AnimatedBatteryCharging> createState() => _AnimatedBatteryChargingState();
}

class _AnimatedBatteryChargingState extends State<AnimatedBatteryCharging>
    with TickerProviderStateMixin {
  late AnimationController _fillAnimationController;
  late AnimationController _chargingAnimationController;
  late Animation<double> _fillAnimation;
  late Animation<double> _chargingAnimation;

  @override
  void initState() {
    super.initState();
    _fillAnimationController = AnimationController(
      duration: widget.animationDuration,
      vsync: this,
    );
    _fillAnimation = Tween<double>(begin: 0, end: widget.percentage / 100)
        .animate(CurvedAnimation(parent: _fillAnimationController, curve: Curves.easeInOut));
    _fillAnimationController.forward();

    _chargingAnimationController = AnimationController(
      duration: const Duration(milliseconds: 1500),
      vsync: this,
    )..repeat(reverse: false);

    _chargingAnimation = Tween<double>(begin: 0, end: 1).animate(
      CurvedAnimation(parent: _chargingAnimationController, curve: Curves.easeInOut),
    );
  }

  @override
  void didUpdateWidget(covariant AnimatedBatteryCharging oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.percentage != widget.percentage) {
      _fillAnimation = Tween<double>(begin: _fillAnimation.value, end: widget.percentage / 100)
          .animate(CurvedAnimation(parent: _fillAnimationController, curve: Curves.easeInOut));
      _fillAnimationController.forward(from: 0);
    }

    if (oldWidget.isCharging != widget.isCharging) {
      if (widget.isCharging) {
        _chargingAnimationController.repeat(reverse: false);
      } else {
        _chargingAnimationController.stop();
      }
    }
  }

  @override
  void dispose() {
    _fillAnimationController.dispose();
    _chargingAnimationController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          AnimatedBuilder(
            animation: Listenable.merge([_fillAnimation, _chargingAnimation]),
            builder: (context, child) {
              return CustomPaint(
                painter: BatteryChargingPainter(
                  fillPercentage: _fillAnimation.value,
                  batteryColor: widget.batteryColor,
                  backgroundColor: widget.backgroundColor,
                  isCharging: widget.isCharging,
                  chargingProgress: _chargingAnimation.value,
                ),
                size: const Size(100, 150),
              );
            },
          ),
          const SizedBox(height: 16),
          AnimatedBuilder(
            animation: _fillAnimation,
            builder: (context, child) {
              return Column(
                children: [
                  Text(
                    '${(_fillAnimation.value * 100).toStringAsFixed(0)}%',
                    style: TextStyle(
                      fontSize: 24,
                      fontWeight: FontWeight.bold,
                      color: widget.textColor,
                    ),
                  ),
                  if (widget.isCharging)
                    Text(
                      'Charging...',
                      style: TextStyle(
                        fontSize: 12,
                        color: Colors.orange,
                        fontWeight: FontWeight.w500,
                      ),
                    ),
                ],
              );
            },
          ),
        ],
      ),
    );
  }
}

class BatteryChargingPainter extends CustomPainter {
  final double fillPercentage;
  final Color batteryColor;
  final Color backgroundColor;
  final bool isCharging;
  final double chargingProgress;

  BatteryChargingPainter({
    required this.fillPercentage,
    required this.batteryColor,
    required this.backgroundColor,
    required this.isCharging,
    required this.chargingProgress,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final Paint backgroundPaint = Paint()
      ..color = backgroundColor
      ..style = PaintingStyle.fill;

    final Paint borderPaint = Paint()
      ..color = Colors.black
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2;

    final Paint fillPaint = Paint()
      ..color = batteryColor
      ..style = PaintingStyle.fill;

    final Paint chargingPaint = Paint()
      ..color = Colors.orange.withOpacity(0.6)
      ..style = PaintingStyle.fill;

    // Draw battery body
    final batteryRect = Rect.fromLTWH(10, 15, size.width - 20, size.height - 30);
    canvas.drawRRect(RRect.fromRectAndRadius(batteryRect, Radius.circular(8)), backgroundPaint);
    canvas.drawRRect(RRect.fromRectAndRadius(batteryRect, Radius.circular(8)), borderPaint);

    // Draw fill
    final fillHeight = (size.height - 30) * fillPercentage;
    final fillRect = Rect.fromLTWH(
      10,
      size.height - 15 - fillHeight,
      size.width - 20,
      fillHeight,
    );
    canvas.drawRRect(RRect.fromRectAndRadius(fillRect, Radius.circular(6)), fillPaint);

    // Draw charging animation
    if (isCharging && fillPercentage < 1.0) {
      final chargeWaveHeight = 8.0;
      final chargeWaveWidth = size.width - 20;
      final chargeWaveY = size.height - 15 - fillHeight - chargeWaveHeight;

      // Draw animated charging wave
      canvas.save();
      canvas.clipRRect(RRect.fromRectAndRadius(fillRect, Radius.circular(6)));

      for (int i = 0; i < 3; i++) {
        final offset = (chargingProgress + i / 3) % 1.0;
        final waveX = (chargeWaveWidth * offset) - chargeWaveWidth;

        final Path wavePath = Path();
        wavePath.moveTo(waveX, chargeWaveY);
        wavePath.quadraticBezierTo(
          waveX + chargeWaveWidth / 4,
          chargeWaveY - chargeWaveHeight,
          waveX + chargeWaveWidth / 2,
          chargeWaveY,
        );
        wavePath.quadraticBezierTo(
          waveX + chargeWaveWidth * 3 / 4,
          chargeWaveY + chargeWaveHeight,
          waveX + chargeWaveWidth,
          chargeWaveY,
        );
        wavePath.lineTo(waveX + chargeWaveWidth, size.height);
        wavePath.lineTo(waveX, size.height);
        wavePath.close();

        canvas.drawPath(wavePath, chargingPaint);
      }

      canvas.restore();
    }

    // Draw terminal (top bump)
    final terminalRect = Rect.fromLTWH(size.width / 2 - 12, 0, 24, 15);
    canvas.drawRRect(RRect.fromRectAndRadius(terminalRect, Radius.circular(4)), borderPaint);
    canvas.drawRRect(RRect.fromRectAndRadius(terminalRect, Radius.circular(4)), backgroundPaint);
  }

  @override
  bool shouldRepaint(BatteryChargingPainter oldDelegate) {
    return oldDelegate.fillPercentage != fillPercentage ||
        oldDelegate.isCharging != isCharging ||
        oldDelegate.chargingProgress != chargingProgress;
  }
}

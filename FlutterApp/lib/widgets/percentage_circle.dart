import 'dart:math' as math;
import 'package:flutter/material.dart';

/// A reusable circular percentage widget.
///
/// - Displays the percentage (0-100) in the center
/// - Fills the circle (pie) according to the value
/// - Optionally shows a donut (ring) instead of a filled pie
/// - Animates when the value changes (enabled by default)
class PercentageCircle extends StatelessWidget {
  final double percentage; // 0..100
  final double size;
  final double strokeWidth;
  final Color backgroundColor;
  final Color fillColor;
  final bool autoColor;
  final TextStyle? textStyle;
  final Duration animationDuration;
  final bool animate;
  final bool donut;

  const PercentageCircle({
    Key? key,
    required this.percentage,
    this.size = 100,
    this.strokeWidth = 16.0,
    this.backgroundColor = const Color(0xFFE0E0E0),
    this.fillColor = Colors.blue,
    this.autoColor = true,
    this.textStyle,
    this.animationDuration = const Duration(milliseconds: 600),
    this.animate = true,
    this.donut = true,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    final effectiveTextStyle = textStyle ?? TextStyle(
      fontSize: size * 0.25,
      fontWeight: FontWeight.bold,
      color: Colors.black,
    );

    final target = percentage.clamp(0.0, 100.0);

    Widget buildPaint(double value) {
      final effectiveFillColor = autoColor ? PercentageCircle._colorForPercentage(value) : fillColor;

      return SizedBox(
        width: size,
        height: size,
        child: CustomPaint(
          size: Size.square(size),
          painter: _PercentagePainter(
            percentage: value,
            strokeWidth: strokeWidth,
            backgroundColor: backgroundColor,
            fillColor: effectiveFillColor,
            donut: donut,
          ),
          child: Center(
            child: Text('${value.round()}%', style: effectiveTextStyle),
          ),
        ),
      );
    }

    if (!animate) return buildPaint(target);

    return TweenAnimationBuilder<double>(
      tween: Tween(begin: 0.0, end: target),
      duration: animationDuration,
      builder: (context, value, child) => buildPaint(value),
    );
  }

  static Color _colorForPercentage(double p) {
    if (p <= 10) return Colors.red;
    if (p >= 40) return Colors.green;
    final t = (p - 10) / 30.0; // 0.0 at 10%, 1.0 at 40%
    return Color.lerp(Colors.red, Colors.green, t)!;
  }
}

class _PercentagePainter extends CustomPainter {
  final double percentage;
  final double strokeWidth;
  final Color backgroundColor;
  final Color fillColor;
  final bool donut;

  _PercentagePainter({
    required this.percentage,
    this.strokeWidth = 16.0,
    this.backgroundColor = const Color(0xFFE0E0E0),
    this.fillColor = Colors.blue,
    this.donut = false,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = size.center(Offset.zero);
    final radius = math.min(size.width, size.height) / 2;

    final sweep = (percentage / 100.0) * 2 * math.pi;
    final startAngle = -math.pi / 2; // start at top

    if (donut) {
      // Draw background ring
      final bgRingPaint = Paint()
        ..color = backgroundColor
        ..style = PaintingStyle.stroke
        ..strokeWidth = strokeWidth
        ..strokeCap = StrokeCap.round;

      final rect = Rect.fromCircle(center: center, radius: radius - strokeWidth / 2);
      // full circle background ring
      canvas.drawArc(rect, 0, 2 * math.pi, false, bgRingPaint);

      // Foreground arc
      if (sweep > 0) {
        final arcPaint = Paint()
          ..color = fillColor
          ..style = PaintingStyle.stroke
          ..strokeWidth = strokeWidth
          ..strokeCap = StrokeCap.round;

        canvas.drawArc(rect, startAngle, sweep, false, arcPaint);
      }
    } else {
      // Background (unfilled)
      final bgPaint = Paint()
        ..color = backgroundColor
        ..style = PaintingStyle.fill;

      canvas.drawCircle(center, radius, bgPaint);

      // Pie (filled sector)
      if (sweep > 0) {
        final path = Path()..moveTo(center.dx, center.dy);
        path.arcTo(Rect.fromCircle(center: center, radius: radius), startAngle, sweep, false);
        path.close();

        final fillPaint = Paint()
          ..color = fillColor
          ..style = PaintingStyle.fill;

        canvas.drawPath(path, fillPaint);
      }
    }
  }

  @override
  bool shouldRepaint(covariant _PercentagePainter oldDelegate) {
    return oldDelegate.percentage != percentage ||
        oldDelegate.fillColor != fillColor ||
        oldDelegate.backgroundColor != backgroundColor ||
        oldDelegate.strokeWidth != strokeWidth ||
        oldDelegate.donut != donut;
  }
}

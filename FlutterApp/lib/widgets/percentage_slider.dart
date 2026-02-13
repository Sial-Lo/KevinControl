import 'package:flutter/material.dart';

/// A simple reusable slider for values in the range 0..100.
///
/// - `value` is clamped to [0,100]
/// - `onChanged` is required and receives the new double value
/// - `divisions` defaults to 100 (1% steps)
/// - `showValueLabel` toggles the textual percent at the end
class PercentageSlider extends StatelessWidget {
  final double value; // 0..100
  final ValueChanged<double> onChanged;
  final int? divisions;
  final bool showValueLabel;
  final Color? activeColor;
  final Color? inactiveColor;
  final String? label;

  const PercentageSlider({
    Key? key,
    required this.value,
    required this.onChanged,
    this.divisions = 100,
    this.showValueLabel = true,
    this.activeColor,
    this.inactiveColor,
    this.label,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    final v = value.clamp(0.0, 100.0);

    return Column(
      mainAxisSize: MainAxisSize.min,
      crossAxisAlignment: CrossAxisAlignment.center,
      children: [
        if (label != null) Padding(
          padding: const EdgeInsets.only(bottom: 8.0),
          child: Text(label!, style: Theme.of(context).textTheme.bodyMedium),
        ),
        Row(
          children: [
            Expanded(
              child: Slider.adaptive(
                value: v,
                min: 0,
                max: 100,
                divisions: divisions,
                label: showValueLabel ? '${v.round()}%' : null,
                activeColor: activeColor,
                inactiveColor: inactiveColor,
                onChanged: onChanged,
              ),
            ),
            if (showValueLabel) ...[
              const SizedBox(width: 8),
              Text('${v.round()}%', style: Theme.of(context).textTheme.bodyMedium),
            ]
          ],
        ),
      ],
    );
  }
}

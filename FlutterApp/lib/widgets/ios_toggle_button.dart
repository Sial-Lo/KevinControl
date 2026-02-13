import 'package:flutter/material.dart';

/// iOS-style toggle button that switches between 0 and 1 on tap.
///
/// - `value` must be 0 or 1
/// - `onChanged` is called with the new int value (0 or 1)
/// - Customizable size and colors
class IoSToggleButton extends StatefulWidget {
  final int value; // 0 or 1
  final ValueChanged<int> onChanged;
  final double width;
  final double height;
  final Color activeColor; // when value == 1
  final Color inactiveColor; // when value == 0
  final Color thumbColor;
  final Duration duration;
  final double padding;
  final String? title;
  final TextStyle? titleStyle;

  // Confirmation Snackbar options
  final bool showConfirmation;
  final String confirmOnMessage;
  final String confirmOffMessage;
  final Duration confirmationDuration;

  // Require explicit user confirmation (dialog) before applying state change
  final bool requireConfirmation;
  final String? confirmDialogTitle;
  final String? confirmDialogContentOn;
  final String? confirmDialogContentOff;
  final String confirmPositiveLabel;
  final String confirmNegativeLabel;

  const IoSToggleButton({
    Key? key,
    required this.value,
    required this.onChanged,
    this.width = 52.0,
    this.height = 32.0,
    this.activeColor = const Color.fromARGB(255, 53, 170, 82), // iOS green
    this.inactiveColor = const Color.fromARGB(255, 197, 52, 52),
    this.thumbColor = Colors.white,
    this.duration = const Duration(milliseconds: 200),
    this.padding = 3.0,
    this.title,
    this.titleStyle,

    // Snackbar confirmation options
    this.showConfirmation = false,
    this.confirmOnMessage = 'Enabled',
    this.confirmOffMessage = 'Disabled',
    this.confirmationDuration = const Duration(milliseconds: 1500),

    // Require explicit user confirmation (dialog) before changing state
    this.requireConfirmation = false,
    this.confirmDialogTitle,
    this.confirmDialogContentOn,
    this.confirmDialogContentOff,
    this.confirmPositiveLabel = 'Yes',
    this.confirmNegativeLabel = 'Cancel',
  })  : assert(value == 0 || value == 1),
        super(key: key);

  @override
  State<IoSToggleButton> createState() => _IoSToggleButtonState();
}

class _IoSToggleButtonState extends State<IoSToggleButton>
    with SingleTickerProviderStateMixin {
  late int _value;

  @override
  void initState() {
    super.initState();
    _value = widget.value;
  }

  @override
  void didUpdateWidget(covariant IoSToggleButton oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.value != widget.value) {
      setState(() => _value = widget.value);
    }
  }

  Future<void> _toggle() async {
    final newValue = _value == 1 ? 0 : 1;

    // If confirmation is required, show a dialog and return early if not confirmed
    if (widget.requireConfirmation) {
      final title = widget.confirmDialogTitle ?? 'Confirm';
      final content = newValue == 1
          ? (widget.confirmDialogContentOn ?? 'Do you want to enable this?')
          : (widget.confirmDialogContentOff ?? 'Do you want to disable this?');

      final confirmed = await showDialog<bool>(
            context: context,
            builder: (ctx) => AlertDialog(
              title: Text(title),
              content: Text(content),
              actions: [
                TextButton(
                  onPressed: () => Navigator.of(ctx).pop(false),
                  child: Text(widget.confirmNegativeLabel),
                ),
                TextButton(
                  onPressed: () => Navigator.of(ctx).pop(true),
                  child: Text(widget.confirmPositiveLabel),
                ),
              ],
            ),
          ) ??
          false;

      if (!confirmed) return;
    }

    setState(() => _value = newValue);
    widget.onChanged(newValue);

    if (widget.showConfirmation) {
      final msg = newValue == 1 ? widget.confirmOnMessage : widget.confirmOffMessage;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(msg),
          duration: widget.confirmationDuration,
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    final trackWidth = widget.width;
    final trackHeight = widget.height;
    final thumbSize = trackHeight - widget.padding * 2;
    final maxThumbLeft = trackWidth - thumbSize - widget.padding;
    final thumbLeft = (_value == 1) ? maxThumbLeft : widget.padding;

    final Widget toggle = Semantics(
      container: true,
      button: true,
      toggled: _value == 1,
      onTap: _toggle,
      child: GestureDetector(
        onTap: _toggle,
        child: AnimatedContainer(
          duration: widget.duration,
          width: trackWidth,
          height: trackHeight,
          curve: Curves.easeInOut,
          decoration: BoxDecoration(
            color: _value == 1 ? widget.activeColor : widget.inactiveColor,
            borderRadius: BorderRadius.circular(trackHeight / 2),
          ),
          child: Stack(
            children: [
              AnimatedPositioned(
                left: thumbLeft,
                top: widget.padding,
                duration: widget.duration,
                curve: Curves.easeInOut,
                child: Container(
                  width: thumbSize,
                  height: thumbSize,
                  decoration: BoxDecoration(
                    color: widget.thumbColor,
                    shape: BoxShape.circle,
                    boxShadow: [
                      BoxShadow(
                        color: Colors.black.withOpacity(0.25),
                        blurRadius: 2,
                        offset: const Offset(0, 1),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );

    if (widget.title == null) return toggle;

    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        toggle,
        SizedBox(height: 6),
        Text(widget.title!, style: widget.titleStyle ?? Theme.of(context).textTheme.bodySmall, textAlign: TextAlign.center),
      ],
    );
  }
}

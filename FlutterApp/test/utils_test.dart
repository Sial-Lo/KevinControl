import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_my_first_app/utils/utils.dart';

void main() {
  test('tryParseInt returns int or null', () {
    expect(tryParseInt('123'), 123);
    expect(tryParseInt('-5'), -5);
    expect(tryParseInt('abc'), isNull);
  });

  test('parseIntOrDefault returns parsed or default', () {
    expect(parseIntOrDefault('10'), 10);
    expect(parseIntOrDefault('x', 7), 7);
    expect(parseIntOrDefault('x'), 0);
  });

  test('parseIntStrict throws on invalid input', () {
    expect(() => parseIntStrict('42'), returnsNormally);
    expect(() => parseIntStrict('x'), throwsFormatException);
  });
}

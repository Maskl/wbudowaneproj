void ButtonPressed(int buttonNum)
{
	if (buttonNum == 0)
		P1OUT ^= LED_0;
	else
		P1OUT ^= LED_1;
}

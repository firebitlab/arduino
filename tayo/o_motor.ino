void motorLR(unsigned char speedL, unsigned char speedR)
{
  analogWrite(pPwmL, speedL);
  analogWrite(pPwmR, speedR);
}

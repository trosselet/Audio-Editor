#pragma once


namespace AudioUtils
{
	inline float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	inline double LinearInterpolation(double y1, double y2, double x)
	{
		return y1 + (x * (y2 - y1));
	}
}
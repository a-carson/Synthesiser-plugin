#pragma once
#include <Eigen/Dense>


using namespace Eigen;

class MoogLadderFilterLinear
{
public:
	MoogLadderFilterLinear()
	{
		x.setZero();
		Id.setIdentity();
		c << 0.0f, 0.0f, 0.0f, 1.0f;
		a << -1.0f, 0.0f, 0.0f, -4 * r, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f;
		A = a;
		b << om0, 0.0f, 0.0f, 0.0f;
	}

	void setParameters(float sampleRate, float cutOff, float resonance)
	{
		fs = sampleRate;
		k = 1 / sampleRate;
		om0 = 2 * 3.1416 * cutOff;
		r = resonance;
		a(0, 3) = -4 * resonance;
		A = a * om0;
		B = Id - k * A;
		b(0, 0) = om0;

		// for Trapezoid
		//Bt1 = Id - k * A * (0.5f);
		//Bt2 = Id + k * A * (0.5f);
		
	}


	float process(float input)
	{
		float out = c * x;

		// Backward Euler
		x = B.inverse() * (x + k * input * b);

		// Trapezoid
		//x = Bt1.inverse() * Bt2 * (x + k * input * b);

		return out;
	}

private:

	// Input parameters
	float fs;			// sample rate [Hz]
	float k;			// time-step [s]
	float om0 = 100.0f;			// cut-off frequency frequency [rad/s]
	float r = 0.5f;			// feedback coeff 0:1

	// 4x4 arrays
	Matrix4f a;			// state matrix
	Matrix4f A;			// state matrix
	Matrix4f Id;		// identity matrix
	Matrix4f B;			// combined matrix: [Id - k*A]
	//Matrix4f Bt1;	    // combined matrix: [Id - k*A/2]
	//Matrix4f Bt2;		// combined matrix: [Id + k*A/2]

	//Matrix4f invB;      // combined matrix: [Id - k*A]

	// 4x1 vectors
	Vector4f b;		    // [om0 0 0 0]
	RowVector4f c;      // [0 0 0 1] - need to transpose
	Vector4f x;			// state vecotor


};
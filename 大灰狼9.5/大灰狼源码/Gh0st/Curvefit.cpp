// Curvefit.cpp: implementation of the CBezierfit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <MATH.H>
#include "Curvefit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static double DistanceError(const double *x, const double *y, const double *Rawdata, int n);
static void Bezier(double u, const double *a, const double *b, 
			double x4, double y4, double &z, double &s);

void CalcBezier(const double *Rawdata, int n, double *Control)
{
	double x[4];
	double y[4];
	double e1,e2, e3;
	int		Retry;
	double x1a,x2a,y1a,y2a;

	x[0] = Rawdata[0];
	y[0] = Rawdata[1];

	x[1] = Rawdata[2];
	y[1] = Rawdata[3];

	x[2] = Rawdata[n-4];
	y[2] = Rawdata[n-3];

	x[3] = Rawdata[n-2];
	y[3] = Rawdata[n-1];

	// seed with linear interpolation...
	x[1] += x[1] - x[0];
	y[1] += y[1] - y[0];
	x[2] += x[2] - x[3];
	y[2] += y[2] - y[3];

	e1 = DistanceError(x, y, Rawdata, n);
	for (Retry = 1; Retry <= 2; Retry++)
	{
//		TRACE("Retry %d\n", Retry);
//		TRACE("      x1       y2       x2       y2    error\n");
		e3 = 0.5;
		x1a = x[1];
		while (fabs(e3) >= 0.01)
		{
			x[1] += (x[1] - x[0])*e3;
			e2 = DistanceError(x, y, Rawdata, n);
			if (e2 == e1)
				break;
			if (e2 > e1)
			{
				x[1] = x1a;
				e3 /=-3;
			}
			else
			{
				e1 = e2;
				x1a = x[1];
			}
		}

		e3 = 0.5;
		y1a = y[1];
		while (fabs(e3) >= 0.01)
		{
			y[1] += (y[1] - y[0])*e3;
			e2 = DistanceError(x, y, Rawdata, n);
			if (e2 == e1)
				break;
			if (e2 > e1)
			{
				y[1] = y1a;
				e3 /=-3;
			}
			else
			{
				e1 = e2;
				y1a = y[1];
			}
		}

		e3 = 0.5;
		x2a = x[2];
		while (fabs(e3) >= 0.01)
		{
			x[2] += (x[2] - x[3])*e3;
			e2 = DistanceError(x, y, Rawdata, n);
			if (e2 == e1)
				break;
			if (e2 > e1)
			{
				x[2] = x2a;
				e3 /=-3;
			}
			else
			{
				e1 = e2;
				x2a = x[2];
			}
		}

		e3 = 0.5;
		y2a = y[2];
		while (fabs(e3) >= 0.01)
		{
			y[2] += (y[2] - y[3])*e3;
			e2 = DistanceError(x, y, Rawdata, n);
			if (e2 == e1)
				break;
			if (e2 > e1)
			{
				y[2] = y2a;
				e3 /=-3;
			}
			else
			{
				e1 = e2;
				y2a = y[2];
			}
		}
	} // for

	Control[0] = x[1];
	Control[1] = y[1];
	Control[2] = x[2];
	Control[3] = y[2];
}

double DistanceError(const double *x, const double *y, 
					 const double *Rawdata, int n)
{
	int i;
	double a[4];
	double b[4];
	double u, u1, u2;
	double z, z1, z2, s, s1;
	double temp;
	double totalerror;
	double stepsize;
	double x4, y4;

	totalerror = 0;
	a[3] = (x[3]-x[0]+3*(x[1]-x[2]))/8;
	b[3] = (y[3]-y[0]+3*(y[1]-y[2]))/8;
	a[2] = (x[3]+x[0]-x[1]-x[2])*3/8;
	b[2] = (y[3]+y[0]-y[1]-y[2])*3/8;
	a[1] = (x[3]-x[0])/2 -a[3];
	b[1] = (y[3]-y[0])/2 -b[3];
	a[0] = (x[3]+x[0])/2 -a[2];
	b[0] = (y[3]+y[0])/2 -b[2];

	stepsize = 2.0/(n);
	s = u1 = z1 = s1 = 0;
	for (i = 2; i <= n-3; i+=2)
	{
		x4 = Rawdata[i];
		y4 = Rawdata[i+1];
		for (u = -1; u <= 1.01; u += stepsize)
		{
			Bezier(u, a, b, x4, y4, z, s);
			if (s == 0)
			{
				u1 = u;
				z1 = z;
				s1 = s;
				break;
			}
			if (u == -1)
			{
				u1 = u;
				z1 = z;
				s1 = s;
			} 
			if (s < s1)
			{
				u1 = u;
				z1 = z;
				s1 = s;
			}
		}
		if (s1 != 0)
		{
			u = u1 + stepsize;
			if (u > 1)
				u = 1 - stepsize;
			Bezier(u, a, b, x4, y4, z, s);
			while (s != 0 && z != 0)
			{
				u2 = u;
				z2 = z;
				temp = z2-z1;
				if (temp != 0)
					u = (z2 * u1-z1*u2)/temp;
				else
					u = (u1 + u2)/2;

				if (u > 1)
					u = 1;
				else if (u < -1)
					u = -1;
				if (fabs(u-u2) < 0.001)
					break;
				u1 = u2;
				z1 = z2;
				Bezier(u, a, b, x4, y4, z, s);
			}
		}
		totalerror += s;
	}
//	TRACE("%8.3lf %8.3lf %8.3lf %8.3lf %8.3lf\n", x[1], y[1], x[2], y[2], totalerror);
	return totalerror;
}

void Bezier(double u, const double *a, const double *b, 
			double x4, double y4, double &z, double &s)
{
	double x, y;
	double dx4,dy4;
	double dx,dy;

	x = a[0] + u*(a[1] + u*(a[2]+u*a[3]));
	y = b[0] + u*(b[1] + u*(b[2]+u*b[3]));
	dx4 = x-x4; dy4 = y-y4;
	dx = a[1] + u * (a[2] + a[2] + u*3*a[3]);
	dy = b[1] + u * (b[2] + b[2] + u*3*b[3]);
	z = dx * dx4 + dy*dy4;
	s = dx4*dx4 + dy4*dy4;
}
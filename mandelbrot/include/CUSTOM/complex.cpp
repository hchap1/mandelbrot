class Complex {
public:
	double a;
	double b;
	Complex(double a, double b) : a(a), b(b) {}

	Complex iter(double oa, double ob) {
		return Complex(a * a - b * b + oa, 2 * a * b + ob);
	}

	bool is_stable(int order) {
		double oa = a;
		double ob = b;
		Complex z = iter(oa, ob);
		for (int i = 1; i < order; i++) {
			z = z.iter(oa, ob);
			if ((z.a - oa) * (z.a - oa) + (z.b - ob) * (z.b - ob) > 4.0) {
				return false;
			}
		}
		return true;
	}
};
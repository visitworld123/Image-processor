void Img::GetOtusThreshold(int nSize) {
	int thre;
	int i, gmin, gmax;
	double dist, f, max;
	int s1, s2, n1, n2, n;
	gmin = 0;
	while (Histogram[gmin] == 0)++gmin;
	gmax = nSize - 1;
	while (Histogram[gmax] == 0)--gmax;
	max = 0;
	thre = 0;
	s1 = n1 = 0;
	for (s2 = n2 = 0, i = gmin; i < gmax; i++) {
		s2 += Histogram[i] * i;
		n2 += Histogram[i];
	}
	for (i = gmin, n = n2; i < gmax; i++) {
		if (!Histogram[i])continue;
		s1 += Histogram[i] * i;
		s2 -= Histogram[i] * i;
		n1 += Histogram[i];
		n2 -= Histogram[i];
		dist = (s1 * 1.0 / n1 - s2 * 1.0 / n2);
		f = dist * dist * (n1 * 1.0 / n) * (n2 * 1.0 / n);
		if (f > max) {
			max = f;
			thre = i;
		}
	}
	OtsuThre = thre;
	cout << "get thre is " << thre << endl;
}

void Img::Cluster(int k) {
	//----------------创建所需空间
	//从1开始存储每个聚类中心
	if (!ClusCenter) { ClusCenter = new double[k+1]; }
    //存储每个灰度值所属类别
	if (!cluster) { cluster = new int[256]; }
	double* preCenter = new double[k+1];//存储前一轮的聚类中心
	//初始化聚类中心
	memset(ClusCenter, 0, sizeof(double)*(k + 1));
	memset(cluster, 0,sizeof(int)*256);
	memset(preCenter, 0xff, sizeof(double)*(k + 1));//前一轮的初始化为很大的值
	bool flag = 0;  //聚类中心差距很小
	int pos = 0;//从pos开始找不为0的位置
	//---------------初始化聚类中心------------------//
	for (int i = 1; i <= k; i++) {
		while (Histogram[pos] == 0)++pos;
		ClusCenter[i] = pos;
		pos++;
	}
	while (!flag) {
		//----------------判断是否够结束条件--------------//
		bool flag1 = 1;
		for (int i = 1; i <= k; i++) {
			double sum;
			sum = abs(preCenter[i] - ClusCenter[i]);
			flag1 = flag1 & (sum < 0.1);
		}
		flag = flag1;
		//--------------动态聚类------------------------//
		
		for (int i = 0; i < 256; i++) {    //对每个灰度值分类
			double min=256;
			int index=0;
			for (int j = 1; j <= k; j++) {  //灰度值到每个聚类中心的距离
				double dis = abs(ClusCenter[j] - i);
				if (dis < min) {
					min = dis;
					index = j;
				}
			}
			cluster[i] = index;//这个灰度值就属于距离第index类
		}
	//存储旧的聚类中心
		for (int i = 1; i <= k; i++) {
			preCenter[i] = ClusCenter[i];
		}
		//计算新的聚类中心
		for (int i = 1; i <= k; i++) {
			int sum = 0;
			int num = 0;
			for (int j = 0; j < 256; j++) {
				if (cluster[j] == i) { sum += j*Histogram[j]; num += Histogram[j]; }
			}
			ClusCenter[i] = sum * 1.0 / num;
		}
	}
	cout << ClusCenter[1] << ClusCenter[2] << endl;
}

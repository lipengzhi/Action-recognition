#ifndef pca_hpp
#define pca_hpp

#include <map>

class PCA {

private:
    typedef struct {
        double coor[3];
    } threed;

    static void adjust_data(std::vector<threed>& d, std::vector<double>& means) {
        
        const int cnts = (int)d.size();
        means.at(0) = 0;
        means.at(1) = 0;
        means.at(2) = 0;
        for (int j=0; j< cnts; ++j) {
            means.at(0) += d.at(j).coor[0];
            means.at(1) += d.at(j).coor[1];
            means.at(2) += d.at(j).coor[2];
        }
        means.at(0) /= cnts;
        means.at(1) /= cnts;
        means.at(2) /= cnts;

        // subtract the mean
        for (int j=0; j<cnts; ++j) {
            d.at(j).coor[0] -= means.at(0);
            d.at(j).coor[1] -= means.at(1);
            d.at(j).coor[2] -= means.at(2);
        }
    }

	static double compute_covariance(const std::vector<threed>& d, int i, int j) {
	   double cov = 0;
	   for (int k=0; k<d.size(); ++k) {
		   cov += d.at(k).coor[i] * d.at(k).coor[j];
	   }
	   return cov / (d.size() - 1);
	}

	static void compute_covariance_matrix(const std::vector<threed> & d, double covar_matrix[][3]) {
        int dim = 3;
        for (int i=0; i<dim; ++i) {
            for (int j=i; j<dim; ++j) {
                covar_matrix[i][j] = compute_covariance(d, i, j);
            }
        }

        // fill the Left triangular matrix
        for (int i=1; i<dim; i++) {
            for (int j=0; j<i; ++j) {
                covar_matrix[i][j] = covar_matrix[j][i];
            }
        }
	}

	static void transpose(const std::vector<threed>& src, std::vector<threed>& target) {
        const int dim1 = (int)src.size();
        const int dim2 = 3;
        for (int i=0; i < dim1; ++i) {
            for (int j=0; j<dim2; ++j) {
                target[j].coor[i] = src[i].coor[j];
            }
        }
	}

	// z = x * y
	static void multiply(const std::vector<threed>& x, const std::vector<threed>& y, std::vector<threed>& z) {
        assert(3 == y.size());
        for (int i=0; i<x.size(); ++i) {
            for (int j=0; j<3; ++j) {
                double sum = 0;
                int d = 3;
                for (int k=0; k<d; k++) {
                    sum += x[i].coor[k] * y[k].coor[j];
                }
                z[i].coor[j] = sum;
            }
        }
    }
    
    /**
     * @brief 求实对称矩阵的特征值及特征向量的雅克比法
     * 利用雅格比(Jacobi)方法求实对称矩阵的全部特征值及特征向量
     * @param pMatrix                长度为n*n的数组，存放实对称矩阵
     * @param nDim                   矩阵的阶数
     * @param pdblVects              长度为n*n的数组，返回特征向量(按列存储)
     * @param dbEps                  精度要求
     * @param nJt                    整型变量，控制最大迭代次数
     * @param pdbEigenValues         特征值数组
     */
    static bool JacbiCor(double * pMatrix,int nDim, double *pdblVects, double *pdbEigenValues, double dbEps,int nJt)
    {
        for(int i = 0; i < nDim; i ++)
        {
            pdblVects[i*nDim+i] = 1.0f;
            for(int j = 0; j < nDim; j ++)
            {
                if(i != j)
                    pdblVects[i*nDim+j]=0.0f;
            }
        }
        
        int nCount = 0;     //迭代次数
        while(1)
        {
            //在pMatrix的非对角线上找到最大元素
            double dbMax = pMatrix[1];
            int nRow = 0;
            int nCol = 1;
            for (int i = 0; i < nDim; i ++)          //行
            {
                for (int j = 0; j < nDim; j ++)      //列
                {
                    double d = fabs(pMatrix[i*nDim+j]);
                    
                    if((i!=j) && (d> dbMax))
                    {
                        dbMax = d;
                        nRow = i;
                        nCol = j;
                    }
                }
            }
            
            if(dbMax < dbEps)     //精度符合要求
                break;
            
            if(nCount > nJt)       //迭代次数超过限制
                break;
            
            nCount++;
            
            double dbApp = pMatrix[nRow*nDim+nRow];
            double dbApq = pMatrix[nRow*nDim+nCol];
            double dbAqq = pMatrix[nCol*nDim+nCol];
            
            //计算旋转角度
            double dbAngle = 0.5*atan2(-2*dbApq,dbAqq-dbApp);
            double dbSinTheta = sin(dbAngle);
            double dbCosTheta = cos(dbAngle);
            double dbSin2Theta = sin(2*dbAngle);
            double dbCos2Theta = cos(2*dbAngle);
            
            pMatrix[nRow*nDim+nRow] = dbApp*dbCosTheta*dbCosTheta +
            dbAqq*dbSinTheta*dbSinTheta + 2*dbApq*dbCosTheta*dbSinTheta;
            pMatrix[nCol*nDim+nCol] = dbApp*dbSinTheta*dbSinTheta +
            dbAqq*dbCosTheta*dbCosTheta - 2*dbApq*dbCosTheta*dbSinTheta;
            pMatrix[nRow*nDim+nCol] = 0.5*(dbAqq-dbApp)*dbSin2Theta + dbApq*dbCos2Theta;
            pMatrix[nCol*nDim+nRow] = pMatrix[nRow*nDim+nCol];
            
            for(int i = 0; i < nDim; i ++)
            {
                if((i!=nCol) && (i!=nRow))
                {
                    int u = i*nDim + nRow;  //p
                    int w = i*nDim + nCol;  //q
                    dbMax = pMatrix[u];
                    pMatrix[u]= pMatrix[w]*dbSinTheta + dbMax*dbCosTheta;
                    pMatrix[w]= pMatrix[w]*dbCosTheta - dbMax*dbSinTheta;
                }
            }
            
            for (int j = 0; j < nDim; j ++)
            {
                if((j!=nCol) && (j!=nRow))
                {
                    int u = nRow*nDim + j;  //p
                    int w = nCol*nDim + j;  //q
                    dbMax = pMatrix[u];
                    pMatrix[u]= pMatrix[w]*dbSinTheta + dbMax*dbCosTheta;
                    pMatrix[w]= pMatrix[w]*dbCosTheta - dbMax*dbSinTheta;
                }
            }
            
            //计算特征向量
            for(int i = 0; i < nDim; i ++)
            {
                int u = i*nDim + nRow;      //p
                int w = i*nDim + nCol;      //q
                dbMax = pdblVects[u];
                pdblVects[u] = (pdblVects[w]*dbSinTheta + dbMax*dbCosTheta);
                pdblVects[w] = (pdblVects[w]*dbCosTheta - dbMax*dbSinTheta);
            }
            
        }
        
        //对特征值进行排序以及重新排列特征向量,特征值即pMatrix主对角线上的元素
        std::map<double,int> mapEigen;
        for(int i = 0; i < nDim; i ++)
        {
            pdbEigenValues[i] = pMatrix[i*nDim+i];
            
            mapEigen.insert(std::make_pair( pdbEigenValues[i],i ) );
        }
        
        double *pdbTmpVec = new double[nDim*nDim];
        std::map<double,int>::reverse_iterator iter = mapEigen.rbegin();
        for (int j = 0; (iter != mapEigen.rend() &&  j < nDim); ++iter,++j)
        {
            for (int i = 0; i < nDim; i ++)
            {
                pdbTmpVec[i*nDim+j] = pdblVects[i*nDim + iter->second];
            }
            
            //特征值重新排列
            pdbEigenValues[j] = iter->first;
        }
        
        //设定正负号
        for(int i = 0; i < nDim; i ++)
        {
            double dSumVec = 0;
            for(int j = 0; j < nDim; j ++)
                dSumVec += pdbTmpVec[j * nDim + i];
            if(dSumVec<0)
            {
                for(int j = 0;j < nDim; j ++)
                    pdbTmpVec[j * nDim + i] *= -1;
            }
        }
        
        memcpy(pdblVects,pdbTmpVec,sizeof(double)*nDim*nDim);
        delete []pdbTmpVec;
        return 1;
    }
    
public:
    static int pca(std::vector<TrackPoint> &vecPt) {
        const int row = (int)vecPt.size();
        const int col = 3;
        
        std::vector<threed> d(row);
        for (int i = 0; i < row; i++) {
            d.at(i).coor[0] = vecPt.at(i).x;
            d.at(i).coor[1] = vecPt.at(i).y;
            d.at(i).coor[2] = vecPt.at(i).z;
        }
        
        std::vector<double> means(col);
        adjust_data(d, means);
        //    cout << "mean: " << means[0] << ", " << means[1] << ", " << means[2] <<endl;
        
        double covar_matrix[3][3];
        compute_covariance_matrix(d, covar_matrix);
        //    cout << "Covariance_matrix: " << covar_matrix[0][0] << ", " << covar_matrix[0][1] << ", " << covar_matrix[0][2]
        //         << ", " << covar_matrix[1][0] << ", " << covar_matrix[1][1] << ", " << covar_matrix[1][2]
        //         << ", " << covar_matrix[2][0] << ", " << covar_matrix[2][1] << ", " << covar_matrix[2][2] <<endl;
        
        //guobo test
        double *pMatrix = NULL;
        pMatrix = new double[9];
        pMatrix[0] = covar_matrix[0][0];
        pMatrix[1] = covar_matrix[0][1];
        pMatrix[2] = covar_matrix[0][2];
        pMatrix[3] = covar_matrix[1][0];
        pMatrix[4] = covar_matrix[1][1];
        pMatrix[5] = covar_matrix[1][2];
        pMatrix[6] = covar_matrix[2][0];
        pMatrix[7] = covar_matrix[2][1];
        pMatrix[8] = covar_matrix[2][2];
        double *pdblVects = NULL;
        pdblVects = new double[9];
        double *pdbEigenValues = NULL;
        pdbEigenValues = new double[3];
        const int dim = col;
        JacbiCor(pMatrix, dim, pdblVects, pdbEigenValues, 0.001,1000);

        //    cout << "The pdblVects:" << endl;
        //    cout << pdblVects[0] << ", " << pdblVects[1] << ", " << pdblVects[2]
        //         << ", " << pdblVects[3] << ", " << pdblVects[4] << ", " << pdblVects[5]
        //         << ", " << pdblVects[6] << ", " << pdblVects[7] << ", " << pdblVects[8]<<endl;
        //    cout << "The pdbEigenValues:" << endl;
        //    cout << pdbEigenValues[0] << ", " << pdbEigenValues[1] << ", " << pdbEigenValues[2]<< endl; //guobo
        
        // restore the old data
        // final_data = RowFeatureVector * RowDataAdjust
        std::vector<threed> final_data(row);
        std::vector<threed> transpose_vct(dim);
        std::vector<threed> eigenvector(dim);
        eigenvector[0].coor[0] = pdblVects[0];
        eigenvector[0].coor[1] = pdblVects[1];
        eigenvector[0].coor[2] = pdblVects[2];
        eigenvector[1].coor[0] = pdblVects[3];
        eigenvector[1].coor[1] = pdblVects[4];
        eigenvector[1].coor[2] = pdblVects[5];
        eigenvector[2].coor[0] = pdblVects[6];
        eigenvector[2].coor[1] = pdblVects[7];
        eigenvector[2].coor[2] = pdblVects[8];
        
        multiply(d,eigenvector, final_data);
        
        //    cout << "the final data" << endl;
        //    cout << "final_data.dim1: " << final_data.dim1() << ", final_data.dim2: " << final_data.dim2()
        //         << ", data_: " << final_data[0][2] <<endl;
        
        //    ofstream outFile;
        //    //outFile.open("/Users/guobo/smart_bracelet/Principal-component-analysis/3d-org_.csv", ios::out);
        //    outFile.open("/Users/guobo/smart_bracelet/Principal-component-analysis/3d-rotation_.csv", ios::out);
        //    for(int i=0;i<row;i++){
        //        outFile << final_data[i].coor[0] <<"," << final_data[i].coor[1] << "," << final_data[i].coor[2] <<endl;
        //    }
        //    outFile.close();
        double x_symbol = 1;
        double y_symbol = 1;
        double z_symbol = 1;
        int gap_x = row/5;    //add gap_x for 123 case.
        if (gap_x < SAMPLE_HZ/3) {
            gap_x = SAMPLE_HZ/3;
        } else if(gap_x > 2*SAMPLE_HZ){
            gap_x = 2*SAMPLE_HZ;
        }
        int gap = row/5;
        if (gap < SAMPLE_HZ/3) {
            gap = SAMPLE_HZ/3;
        } else if(gap > SAMPLE_HZ){
            gap = SAMPLE_HZ;
        }
        if(gap_x > (final_data.size()-1)){
            delete[] pMatrix;
            delete[] pdblVects;
            delete[] pdbEigenValues;
            return -1;
        }
//        double val = final_data.at(gap_x).coor[0] - final_data.at(0).coor[0];
//        if (val < 0) {
//            x_symbol = -1;
//        }
//        val = final_data.at(gap).coor[1] - final_data.at(0).coor[1];
//        if (val < 0) {
//            y_symbol = -1;
//        }
//        val = final_data.at(gap).coor[2] - final_data.at(0).coor[2];
//        if (val < 0) {
//            z_symbol = -1;
//        }
        for (int i = 0; i < row; i++) {
            vecPt.at(i).x = x_symbol * final_data.at(i).coor[0];
            vecPt.at(i).y = y_symbol * final_data.at(i).coor[1];
            vecPt.at(i).z = z_symbol * final_data.at(i).coor[2];
        }
        
        delete[] pMatrix;
        delete[] pdblVects;
        delete[] pdbEigenValues;
        return 0;
    }
};

#endif /* pca_hpp */

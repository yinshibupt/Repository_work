void RayTracingCheckFS::RayTracingCompute(int LAC,int CI,double Dist) 
{
	 this->LAC=LAC;
	 this->CI=CI;
	//得到一段路径上的大误差样本 
	 int dist=Dist;
	 String^sql="select * from MRPreprocessbefore where  dist>="+dist+" and longitude is not null";
	 System::Data::DataTable^ dt1=da->FetchData(sql);
	 //没有的话不需要做修正
	 if(dt1->Rows ->Count ==0)
		return;
	//记录大误差集合所在的栅格标识
	 ArrayList^GXID_GYID=gcnew ArrayList();
	 ArrayList^GXID_GYIDISNotExist=gcnew ArrayList();
	 //记录误差值 -20121113
	 ArrayList^dist1=gcnew ArrayList();
	// String^time=Convert::ToString (System::DateTime ::Now );
	 for(int i=0;i<dt1->Rows ->Count ;i++)
	 {
		 //判断此条大误差mr是否与该小区相关
		for(int j=0;j<=6;j++)
		{
			if(Convert::ToInt32 (dt1->Rows [i]["lac"+j])==LAC&&Convert::ToInt32 (dt1->Rows [i]["CI"+j])==CI&&dt1->Rows [i]["Longitude"]->ToString ()!="")
			{
				//在判断与该小区相关的大误差mr是否在该小区覆盖范围内
				if(!this->IsIntheLACCI (Convert::ToInt32 (dt1->Rows [i]["gxid"]),Convert::ToInt32 (dt1->Rows [i]["gyid"]) ))
				{

					String^id=dt1->Rows [i]["gxid"]+"_"+dt1->Rows [i]["gyid"];
					if(GXID_GYID->Contains (id))
					{
						break;
					}
					else
					{
						GXID_GYID->Add (id);
						 String^time=Convert::ToString (System::DateTime ::Now );
						String^insert="insert into tbFSAdjustLog values('"+time+"',"+LAC+","+CI+","+dt1->Rows [i]["gxid"]+","+dt1->Rows [i]["gyid"]+",1)";
						da->modifydb (insert);
						break;
					}
				}
				else
				{
					String^id=dt1->Rows [i]["gxid"]+"_"+dt1->Rows [i]["gyid"];
					if(GXID_GYIDISNotExist->Contains (id))
						break;
					else
					{
						GXID_GYIDISNotExist->Add (id);
						 String^time=Convert::ToString (System::DateTime ::Now );
						String^insert="insert into tbFSAdjustLog values('"+time+"',"+LAC+","+CI+","+dt1->Rows [i]["gxid"]+","+dt1->Rows [i]["gyid"]+",0)";
						da->modifydb (insert);
						break;
					}
				}
			}
		}
	}
	//为进度条赋值、显示模型计算速度
   /*this->progressBar1 ->Maximum =GXID_GYID->Count;
   this->progressBar1 ->Minimum =0;
   this->progressBar1 ->Value =0;*/
	 /*addgis4::progressbar ^bar=gcnew progressbar();
			bar->button1 ->Visible =false;
			bar->Show ();
			bar->progressBar1 ->Maximum =GXID_GYID->Count+4;
			bar->progressBar1 ->Minimum =0;*/
	 DataTable ^ dtFS1_Grid=da->FetchData("select * from tbfs1,tbGrid where tbfs1.GXID=tbGrid.GXID and tbfs1.GYID=tbGrid.GYID");

	//矫正栅格去重，add by ys
	 System::Collections::Generic::Dictionary<String^,int>^ grid_num=gcnew System::Collections::Generic::Dictionary<String^,int>();
	 /*if(Convert::ToString (reader["LAC"+i])!="-1")
	 {
		 String^ lac_ci=Convert::ToString (reader["LAC"+i])+"_"+Convert::ToString (reader["CI"+i]);
		 if(cell_num->ContainsKey (lac_ci))
			 ++cell_num[lac_ci];
		 else if(!cell_num->ContainsKey (lac_ci))
			 cell_num->Add (lac_ci,1);
	 }*/
	 int k = GXID_GYID->Count;
	 String^ s=Convert::ToString(LAC)+"_"+Convert::ToString(CI);
   for(int i=0;i<GXID_GYID->Count ;i++)
   {
	   //逐条对栅格进行修正
		 cli::array <String^>^split = GXID_GYID[i]->ToString ()->Split(gcnew cli::array <Char>{'_'});
		 errMRGXID=Convert::ToInt32 (split[0]);
		 errMRGYID=Convert::ToInt32 (split[1]);
//----add by ys添加判断栅格是否已经要处理，只处理一次，用key判断
		 String^ gridxy=Convert::ToString (errMRGXID)+"_"+Convert::ToString(errMRGYID);
		 if(!grid_num->ContainsKey(gridxy))
			 grid_num->Add(gridxy,1);
		 else ++grid_num[gridxy];
//----end add
//////////////////////////计算误差平均值/////////////////////-20121113
		 String^ distavgs="select avg(dist) ,stdevp(dist) from MRPreprocessbefore where gxid="+errMRGXID+" and gyid="+errMRGYID+" and dist>"+dist;
		System::Data::DataTable^ distavg=da->FetchData(distavgs);
		float ave= Convert::ToDouble (distavg->Rows [0][0]);
		float std= Convert::ToDouble (distavg->Rows [0][1]);
//////////////////////////计算误差平均值/////////////////////
		 //删除原来路径中对该栅格的记录
		 sql="delete from tbRayTrace where LAC="+LAC+" and CI="+CI+" and GXID="+errMRGXID+" and GYID="+errMRGYID;
		 da->ExecuteNonQuery(sql);
		//避免小区不存在的情况
		 String^isexist="select * from tbCell where LAC="+LAC+" and "+"CI="+CI;
		 System::Data::DataTable^ dtisexist=da->FetchData(isexist);
		 if(dtisexist->Rows->Count<=0)
			 return; 
		 //小区信息
		 CellInfo^ cellInfo=gcnew CellInfo(da,bmm,LAC,CI,frequncy,directCoefficient,reflectCoefficient,diffractCoefficient,0);
		 //大误差MR所在位置
		 sql="select * from tbGrid where GXID="+errMRGXID+" and GYID="+errMRGYID;
		 System::Data::DataTable^ dt=da->FetchData(sql);
		 if(dt->Rows->Count==0)
		 {
			 continue;
			 /*MessageBox::Show("不存在该网格！");
			 return;*/
		 }
		//得到栅格的中心经纬度
		 MR^ errMR=gcnew MR();
		 errMR->GXID=Convert::ToInt32(dt->Rows[0]["GXID"]);
		 errMR->GYID=Convert::ToInt32(dt->Rows[0]["GYID"]);
		 errMR->CLong=Convert::ToDouble(dt->Rows[0]["CLong"]);
		 errMR->CLat=Convert::ToDouble(dt->Rows[0]["CLat"]);
		 
//------------------------------------------add by ys  
		 double MRaddLon,MRaddLat,mrlon,mrlat,MRaddLon1,MRaddLat1;
		 mrlon=Convert::ToDouble(dt->Rows[0]["CLong"]);
		 mrlat=Convert::ToDouble(dt->Rows[0]["CLat"]);
		 
		 double celllon = Convert::ToDouble(dtisexist->Rows[0]["Longitude"]);
		 double celllat = Convert::ToDouble(dtisexist->Rows[0]["Latitude"]);
		 double anglecell = CJWDHelper::angle(celllon,celllat,mrlon,mrlat);

		 double angle_dir_right = (anglecell+45)<360?(anglecell+45):(anglecell-315);
		 double angle_dir_left = (anglecell-45)<0?(anglecell+315):anglecell-45;

		//添加加入点判断逻辑，注意这里要更新locatingsamples表
		 cellstatistics cellinfo1;
		 System::Data::DataTable ^dtGrid=gcnew System::Data ::DataTable();
		 String^ FrimGrid="select * from LocatingSamples where LAC_CI='"+Convert::ToString(LAC)+"-"+Convert::ToString(CI)+"'";
		 dtGrid=da->FetchData(FrimGrid);

		 int fs180r=0,fs180l=0;
		 for(int h=0;h<dtGrid->Rows ->Count ;h++)
		{
			if(cellinfo1.inTheCellCover(celllon,celllat,(double)dtGrid->Rows [h]["Longitude"],(double)dtGrid->Rows[h]["Latitude"],angle_dir_right,90)) //右瓣
			{
				fs180r=fs180r+1;
			}
			if(cellinfo1.inTheCellCover(celllon,celllat,(double)dtGrid->Rows [h]["Longitude"],(double)dtGrid->Rows[h]["Latitude"],angle_dir_left,90))
			{
				fs180l=fs180l+1;	
			}
		}
		
		 double angle1 = (anglecell-90)>0?anglecell:anglecell+90;
		 int flag;
		//左右瓣经纬度判断逻辑
		 if(fs180r>=fs180l) flag=-1;
		 else flag=1;
		 if(anglecell>=0 && anglecell<90) 
		 {
			MRaddLon=errMR->CLong+0.001* Math::Abs(Math::Cos(angle1))*flag;
			MRaddLat=errMR->CLat-0.001*Math::Abs(Math::Sin(angle1))*flag;
		 }else if(anglecell>=90 && anglecell<180) 
		 {
			 MRaddLon=errMR->CLong-0.001* Math::Abs(Math::Cos(angle1))*flag;
			 MRaddLat=errMR->CLat-0.001*Math::Abs(Math::Sin(angle1))*flag;
		 }else if(anglecell>=90 && anglecell<180)
		 {
			 MRaddLon=errMR->CLong-0.001* Math::Abs(Math::Cos(angle1))*flag;
			 MRaddLat=errMR->CLat+0.001*Math::Abs(Math::Sin(angle1));
		 }else{
			 MRaddLon=errMR->CLong+0.001* Math::Abs(Math::Cos(angle1))*flag;
			 MRaddLat=errMR->CLat+0.001*Math::Abs(Math::Sin(angle1))*flag;
		 }

//		 MRaddLon1=errMR->CLong-0.001* Math::Abs(Math::Cos(angle1));
//		 MRaddLat1=errMR->CLat-0.001*Math::Abs(Math::Sin(angle1));

		 String^ sql_mradd="select * from tbGrid where maxlong>="+MRaddLon+" and minlong<="+MRaddLon+" and maxlat>="+MRaddLat+" and minlat<="+MRaddLat;
		 System::Data::DataTable^ dt_mradd=da->FetchData(sql_mradd);
		
		 MR^ errMR1=gcnew MR();
		 errMR1->GXID=Convert::ToInt32(dt_mradd->Rows[0]["GXID"]);
		 errMR1->GYID=Convert::ToInt32(dt_mradd->Rows[0]["GYID"]);
		 errMR1->CLong=MRaddLon;
		 errMR1->CLat=MRaddLat;

//--------------------------add by ys-----------------


		//计算在建筑物坐标系的经纬度
		 MapInfo::Geometry::DPoint dPoint=bmm->PntFrmLongLatToBlding(MapInfo::Geometry::DPoint(errMR->CLong,errMR->CLat));
		 InGeometry::IPoint^ p=gcnew InGeometry::PointClass();
		 p->X=dPoint.x;
		 p->Y=dPoint.y;
		 p->Z=0;
		 //add by ys 射线在增加一条。
		 MapInfo::Geometry::DPoint dPoint1=bmm->PntFrmLongLatToBlding(MapInfo::Geometry::DPoint(MRaddLon,MRaddLat));
		 InGeometry::IPoint^ p1=gcnew InGeometry::PointClass();
		 p1->X=dPoint1.x;
		 p1->Y=dPoint1.y;
		 p1->Z=0;
		 //-------------------

		 //////反射////////
		 InterferenceAnalysis^ ia=gcnew InterferenceAnalysis(cellInfo,reflectionNum,diffractionNum,da,bmm);
		 //ia->SingleRayAnalysis(cellInfo->SourcePoint,p);
		 //ia->interferenceAnalysis(cellInfo->SourcePoint,p);
		 //ia->interferenceAnalysisTrace(cellInfo->SourcePoint,p,errMR);

		 //////绕射////////
		 DiffractedRayAnalysis^ dra=gcnew DiffractedRayAnalysis(cellInfo,reflectionNum,diffractionNum,sideSplitUnit,da,bmm);
		 //dra->diffractedRayAnalysisSingleV(cellInfo->SourcePoint,p,this->checkBox1->Checked);
		 //dra->diffractedRayAnalysisTrace(cellInfo->SourcePoint,p,errMR,this->computeVSide );
		 Interf_Diff_Line^ idl=gcnew Interf_Diff_Line(cellInfo,ia,dra,da,bmm,dtFS1_Grid);
		 //三维射线跟踪分析函数，输入（小区，mr所在栅格经纬度，大误差mr，这些同一栅格内mr的平均定位误差）
		
		 
 		 idl->Analysis(cellInfo->SourcePoint,p,errMR,ave,std,5,grid_num);
		 idl->Analysis(cellInfo->SourcePoint,p1,errMR1,ave,std,5,grid_num);
		 ///更新进度条
		 /*this->progressBar1 ->Value++;this->progressBar1 ->Refresh ();*/
		/* bar->progressBar1 ->Value++;
		 bar->progressBar1 ->Refresh ();*/
	}
	/*System::Collections::Generic::KeyValuePair<String^, int>^ kvp;
	for each(kvp in grid_num)
    {   
		String^ kvp_key = kvp->Key;
		int kvp_value = kvp->Value;
		String^insert="insert into tbGrid_temp_num values("+LAC+","+CI+",'"+kvp_key +"',"+kvp_value+")";
		da->modifydb(insert);
    }  */
	
//bar->progressBar1 ->Value =bar->progressBar1 ->Maximum ;
//			bar->progressBar1 ->Refresh ();
//			bar->button1 ->Visible =true;
//			bar->Close();
	//MessageBox::Show ("完成！");
	//////修改tbfs中的对应小区的信号强度////////
   if(GXID_GYID->Count >0)
		this->checkFeatureSample ();
}
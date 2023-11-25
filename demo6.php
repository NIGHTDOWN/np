<?php



function p($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    exit;
}


class calcpaper
{

	//成品要求
    public $intPrintProduct = [
        'width' => 210, //成品规格-宽
        'height' => 285, //成品规格-高
        'num' => 100000, //成品数量10万
        'weight' => 200, //200克    
        'printingSurface' => 2, //印面 1-单面，2-双面
        'printingContent' => 2, //1-正反面内容相同，2-正反面内容不同
    ];

	public $intPrint = [
        'bleedingSite' => 3,//出血位宽度
        'bitePosition' => 12//咬口位宽度
    ];

    public $intPaper = [
        'width' => 1194,//纸宽度
        'height' => 889//纸高度
    ];

     public $intProductBleedingSite = [
        'width' => 0, //成品规格加出血位-宽
        'height' => 0, //成品规格加出血位-高
    ];

	public function setPro($h, $w, $ww, $num)
    {
        $this->intPrintProduct['height'] = $h;
        $this->intPrintProduct['width'] = $w;
        $this->intPrintProduct['weight'] = $ww;
        $this->intPrintProduct['num'] = $num;


        $this->intProductBleedingSite['width']=$this->intPrintProduct['width']+$this->intPrint['bleedingSite']*2;
		$this->intProductBleedingSite['height']=$this->intPrintProduct['height']+$this->intPrint['bleedingSite']*2;
    }


    // 排版方法
	public function typesettingMethod(){
		// p($this->intProductBleedingSite);
		// 横/竖排版
		$re=$this->simpleTypeSetting();
		// p($re);
		// 左A右B排版
		$this->leftRightTypeSetting();
		// 上A下B排版
		$this->upDownTypeSetting();
	}

	// 横/竖排版
	public function simpleTypeSetting(){
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['width'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['height'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度

		// echo "$paperWidth * $paperHeight <br>";
		//横排    
		$tmpW = intval(floor($paperWidth / $bleedingSiteW));
		$tmpH = intval(floor($paperHeight / $bleedingSiteH));
		$tmpNum = floor($paperWidth / $bleedingSiteW) * floor($paperHeight / $bleedingSiteH);

		//竖排
		$tmpW2 = intval(floor($paperWidth / $bleedingSiteH));
		$tmpH2 = intval(floor($paperHeight / $bleedingSiteW));
		$tmpNum2 = $tmpW2 * $tmpH2;

		//使用宽和高排数
		$typesettingW = 0;
		$typesettingH = 0;
		$typesettingNum = 0;

		// 排版展开规格
		$unfoldW=0;//宽
		$unfoldH=0;//高

		$typesettingShape = ''; //排版形状
		$area=0;//面积
		if ($tmpNum >= $tmpNum2) {
		    //横排
		    $typesettingX = $tmpW;
		    $typesettingY = $tmpH;
		    $typesettingNum = $tmpNum;
		    $typesettingW = $bleedingSiteW;
		    $typesettingH = $bleedingSiteH;
		    // $typesettingShape = $typesettingW>=$typesettingH?'横排':'竖排';
		    $typesettingShape ='横排';
		    $unfoldW=$tmpW*$typesettingW;
		    $unfoldH=$tmpH*$typesettingH;
		    $unfoldH=$unfoldH+$bitePosition-$bleedingSite;

		    $area=$unfoldW*$unfoldH;
		} else {
		    //竖排
		    $typesettingX = $tmpW2;
		    $typesettingY = $tmpH2;
		    $typesettingNum = $tmpNum2;
		    $typesettingW = $bleedingSiteH;
		    $typesettingH = $bleedingSiteW;
		    // $typesettingShape = $typesettingW>=$typesettingH?'横排':'竖排';
		    $typesettingShape ='竖排';
		    $unfoldW=$tmpW2*$typesettingW;
		    $unfoldH=$tmpH2*$typesettingH;
		    $unfoldH=$unfoldH+$bitePosition-$bleedingSite;
		    $area=$unfoldW*$unfoldH;
		}		
		
		$description="横/竖排版- 排版：$typesettingShape ,成品规格：$typesettingW mm * $typesettingH mm（含出血位）,横排数量：$typesettingX 个,竖排数量：$typesettingY 个,排版数量：$typesettingNum 个,排版展开规格：$unfoldW mm * $unfoldH mm<br><br>";
		echo $description;

		return $data=[
				'typesettingShape'=>$typesettingShape,
				'typesettingW'=>$typesettingW,
				'typesettingH'=>$typesettingH,
				'typesettingX'=>$typesettingX,
				'typesettingY'=>$typesettingY,
				'typesettingNum'=>intval($typesettingNum),
				'unfoldW'=>$unfoldW,
				'unfoldH'=>$unfoldH,
				'area'=>$area,
				'description'=>$description
				];

	}

	

	// 左竖右横排版-右边列数	
	// numXRight 右边列数 竖排 
	public function leftRightTypeSettingVertical($numXRight=1){	
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['width'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['height'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度

		// $numXRight=2;
		// 左边
		$numXLeft=floor(($paperWidth-$bleedingSiteH*$numXRight)/$bleedingSiteW);//左边横向个数
		$typesettingWLeft= $numXLeft*$bleedingSiteW;//左边宽度
		$numYLeft=floor($paperHeight/$bleedingSiteH);//左边竖向个数
		$typesettingHLeft=$numYLeft*$bleedingSiteH;//左边高度
		// p([$numXLeft,$typesettingWLeft,$numYLeft,$typesettingHLeft]);
		$numYRight=floor($paperHeight/$bleedingSiteW);//右边竖向个数
		$typesettingHRight=$numYRight*$bleedingSiteW;//右边高度
		$typesettingWRight=$numXRight*$bleedingSiteH;//右边宽度
		$numLeft=$numXLeft*$numYLeft;//左边个数总数
		$numRight=$numXRight*$numYRight;//右边个数总数


		$unfoldW=$numXLeft*$bleedingSiteW+$numXRight*$bleedingSiteH;//展开宽度
		$unfoldH=$typesettingHLeft>=$typesettingHRight?$typesettingHLeft:$typesettingHRight;//展开高度
		$unfoldH=$unfoldH+$bitePosition-$bleedingSite;//展开高度加1个咬口
		$area=$unfoldW*$unfoldH;//展开面积

		$description= "左竖右横排,右边排{$numXRight}列　排版展开规格：$unfoldW mm * $unfoldH mm(含1个咬口)<br>".
		"左边竖排:成品规格: $typesettingWLeft mm * $typesettingHLeft mm （含出血位）,横排数量：$numXLeft ,竖排数量： $numYLeft ,排版数量： $numLeft  , ".
		"右边横排: 成品规格:$typesettingWRight  mm * $typesettingHRight mm （含出血位）,横排数量： $numXRight ,竖排数量： $numYRight ,排版数量： $numRight    <br><br>";
		// echo $description;

		return $data=[
				// 'typesettingShape'=>$typesettingShape,				
				'typesettingW'=>$unfoldW,
				'typesettingH'=>$unfoldH,
				// 'typesettingX'=>$typesettingX,
				// 'typesettingY'=>$typesettingY,
				'typesettingNum'=>intval($numLeft+$numRightar),
				// 'unfoldW'=>$unfoldW,
				// 'unfoldH'=>$unfoldH,
				'area'=>$area,
				'description'=>$description,
				'part'=>[
					'left'=>[
							'width'=>$typesettingWLeft,
							'height'=>$typesettingHLeft,
							'numX'=>$numXLeft,
							'numY'=>$numYLeft,
							'num'=>$numLeft
						],
					'right'=>[
							'width'=>$typesettingH,
							'height'=>$typesettingW,
							'numX'=>$numXRight,
							'numY'=>$numYRight,
							'num'=>$numRight
						],
					]
				];
	}

	// 左横右竖排版-右边列数 	
	// numXRight 右边列数 横排
	public function leftRightTypeSettingHorizontal($numXRight=1){ 	
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['height'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['width'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度

		// $numXRight=2;
		// 左边
		$numXLeft=floor(($paperWidth-$bleedingSiteH*$numXRight)/$bleedingSiteW);//左边横向个数
		$typesettingWLeft= $numXLeft*$bleedingSiteW;//左边宽度
		$numYLeft=floor($paperHeight/$bleedingSiteH);//左边竖向个数
		$typesettingHLeft=$numYLeft*$bleedingSiteH;//左边高度
		// p([$numXLeft,$typesettingWLeft,$numYLeft,$typesettingHLeft]);
		$numYRight=floor($paperHeight/$bleedingSiteW);//右边竖向个数
		$typesettingHRight=$numYRight*$bleedingSiteW;//右边高度
		$typesettingWRight=$numXRight*$bleedingSiteH;//右边宽度
		$numLeft=$numXLeft*$numYLeft;//左边个数总数
		$numRight=$numXRight*$numYRight;//右边个数总数


		$unfoldW=$numXLeft*$bleedingSiteW+$numXRight*$bleedingSiteH;//展开宽度
		$unfoldH=$typesettingHLeft>=$typesettingHRight?$typesettingHLeft:$typesettingHRight;//展开高度
		$unfoldH=$unfoldH+$bitePosition-$bleedingSite;//展开高度加1个咬口
		$area=$unfoldW*$unfoldH;//展开面积

		$description= "左横右竖排,右边排{$numXRight}列　排版展开规格：$unfoldW mm * $unfoldH mm(含1个咬口)<br>".
		"左边横排:成品规格: $typesettingWLeft mm * $typesettingHLeft mm （含出血位）,横排数量：$numXLeft ,竖排数量： $numYLeft ,排版数量： $numLeft  , ".
		"右边竖排: 成品规格:$typesettingWRight  mm * $typesettingHRight mm （含出血位）,横排数量： $numXRight ,竖排数量： $numYRight ,排版数量： $numRight    <br><br>";
		// echo $description;

		return $data=[
				// 'typesettingShape'=>$typesettingShape,				
				'typesettingW'=>$unfoldW,
				'typesettingH'=>$unfoldH,
				// 'typesettingX'=>$typesettingX,
				// 'typesettingY'=>$typesettingY,
				'typesettingNum'=>intval($numLeft+$numRight),
				// 'unfoldW'=>$unfoldW,
				// 'unfoldH'=>$unfoldH,
				'area'=>$area,
				'description'=>$description,
				'part'=>[
					'left'=>[
							'width'=>$typesettingWLeft,
							'height'=>$typesettingHLeft,
							'numX'=>$numXLeft,
							'numY'=>$numYLeft,
							'num'=>$numLeft
						],
					'right'=>[
							'width'=>$typesettingH,
							'height'=>$typesettingW,
							'numX'=>$numXRight,
							'numY'=>$numYRight,
							'num'=>$numRight
						],
					]
				];
	}

	// 上竖下横排版-下边行数 竖向
	public function upDownTypeSettingVertical($numYDown=1){		
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['width'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['height'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度

		// $numXRight=2;
		// 上边
		$numXUp=floor($paperWidth/$bleedingSiteW);//上边横向个数
		$typesettingWUp= $numXUp*$bleedingSiteW;//上边宽度
		$numYUp=floor(($paperHeight-$bleedingSiteW*$numYDown)/$bleedingSiteH);//上边竖向个数
		$typesettingHUp=$numYUp*$bleedingSiteH;//上边高度
		// p([$numXLeft,$typesettingWLeft,$numYLeft,$typesettingHLeft]);
		
		// 下边
		$numXDown=floor($paperHeight/$bleedingSiteH);//下边横向个数
		$typesettingHDown=$numYDown*$bleedingSiteW;//下边高度
		$typesettingWDown=$numXDown*$bleedingSiteH;//下边宽度
		$numUp=$numXUp*$numYUp;//上边个数总数
		$numDown=$numXDown*$numYDown;//下边个数总数


		$unfoldW=$typesettingWUp>=$typesettingWDown?$typesettingWUp:$typesettingWDown;//展开宽度
		$unfoldH=$typesettingHUp+$typesettingHDown;//展开宽度
		
		$unfoldH=$unfoldH+$bitePosition-$bleedingSite;//展开高度加1个咬口
		$area=$unfoldW*$unfoldH;//展开面积

		$description= "上竖下横排,下边排{$numYDown}行　排版展开规格：$unfoldW mm * $unfoldH mm(含1个咬口)<br>".
		"上边竖排:展开规格: $typesettingWUp mm * $typesettingHUp mm （含出血位）,横排数量：$numXUp ,竖排数量： $numYUp ,排版数量： $numUp  , ".
		"下边横排: 展开规格:$typesettingWDown  mm * $typesettingHDown mm （含出血位）,横排数量： $numXDown ,竖排数量： $numYDown ,排版数量： $numDown    <br><br>";
		// echo $description;

		return $data=[
				// 'typesettingShape'=>$typesettingShape,				
				'typesettingW'=>$unfoldW,
				'typesettingH'=>$unfoldH,
				// 'typesettingX'=>$typesettingX,
				// 'typesettingY'=>$typesettingY,
				'typesettingNum'=>intval($numUp+$numDown),
				// 'unfoldW'=>$unfoldW,
				// 'unfoldH'=>$unfoldH,
				'area'=>$area,
				'description'=>$description,
				'part'=>[
					'up'=>[
							'width'=>$typesettingWUp,
							'height'=>$typesettingHUp,
							'numX'=>$numXUp,
							'numY'=>$numYUp,
							'num'=>$numUp
						],
					'down'=>[
							'width'=>$typesettingWDown,
							'height'=>$typesettingHDown,
							'numX'=>$numXDown,
							'numY'=>$numYDown,
							'num'=>$numDown
						],
					]
				];
	}

	// 上竖下横排版-下边行数 横向
	public function upDownTypeSettingHorizontal($numYDown=1){		
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['height'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['width'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度

		// $numXRight=2;
		// 上边
		$numXUp=floor($paperWidth/$bleedingSiteW);//上边横向个数
		$typesettingWUp= $numXUp*$bleedingSiteW;//上边宽度
		$numYUp=floor(($paperHeight-$bleedingSiteW*$numYDown)/$bleedingSiteH);//上边竖向个数
		$typesettingHUp=$numYUp*$bleedingSiteH;//上边高度
		// p([$numXLeft,$typesettingWLeft,$numYLeft,$typesettingHLeft]);
		
		// 下边
		$numXDown=floor($paperHeight/$bleedingSiteH);//下边横向个数
		$typesettingHDown=$numYDown*$bleedingSiteW;//下边高度
		$typesettingWDown=$numXDown*$bleedingSiteH;//下边宽度
		$numUp=$numXUp*$numYUp;//上边个数总数
		$numDown=$numXDown*$numYDown;//下边个数总数


		$unfoldW=$typesettingWUp>=$typesettingWDown?$typesettingWUp:$typesettingWDown;//展开宽度
		$unfoldH=$typesettingHUp+$typesettingHDown;//展开宽度
		
		$unfoldH=$unfoldH+$bitePosition-$bleedingSite;//展开高度加1个咬口
		$area=$unfoldW*$unfoldH;//展开面积

		$description= "上横下竖排,下边排{$numYDown}行　排版展开规格：$unfoldW mm * $unfoldH mm(含1个咬口)<br>".
		"上边横排:展开规格: $typesettingWUp mm * $typesettingHUp mm （含出血位）,横排数量：$numXUp ,竖排数量： $numYUp ,排版数量： $numUp  , ".
		"下边竖排: 展开规格:$typesettingWDown  mm * $typesettingHDown mm （含出血位）,横排数量： $numXDown ,竖排数量： $numYDown ,排版数量： $numDown    <br><br>";
		// echo $description;

		return $data=[
				// 'typesettingShape'=>$typesettingShape,				
				'typesettingW'=>$unfoldW,
				'typesettingH'=>$unfoldH,
				// 'typesettingX'=>$typesettingX,
				// 'typesettingY'=>$typesettingY,
				'typesettingNum'=>intval($numUp+$numDown),
				// 'unfoldW'=>$unfoldW,
				// 'unfoldH'=>$unfoldH,
				'area'=>$area,
				'description'=>$description,
				'part'=>[
					'up'=>[
							'width'=>$typesettingWUp,
							'height'=>$typesettingHUp,
							'numX'=>$numXUp,
							'numY'=>$numYUp,
							'num'=>$numUp
						],
					'down'=>[
							'width'=>$typesettingWDown,
							'height'=>$typesettingHDown,
							'numX'=>$numXDown,
							'numY'=>$numYDown,
							'num'=>$numDown
						],
					]
				];
	}


	// 左A右B排版
	public function leftRightTypeSetting(){
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['width'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['height'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度


		// 计算左右排，并且取出最优排
		$useArea=0;//使用面积
		$userNum=0;//使用数量
		$selectData=[];
	
		for($i=1;$i<3;$i++){
			// 左竖右横排版-右边列数 竖向
			$re=$this->leftRightTypeSettingVertical($i);
			if($re){
				if($useArea){
					if($re['typesettingNum']>$userNum){
							$useArea=$re['area'];
							$userNum=$re['typesettingNum'];
							$selectData=$re;
					}elseif($re['typesettingNum']==$userNum){
						// p($re);
						if($re['area']<$useArea){
							$useArea=$re['area'];
							$userNum=$re['typesettingNum'];
							$selectData=$re;
						}
					}			
				}else{
					$useArea=$re['area'];
					$userNum=$re['typesettingNum'];
					$selectData=$re;
				}
			}			
		}
		// echo $selectData['description'];
		for($i=1;$i<3;$i++){
			// 左横右竖排版-右边列数 横向
			$re=$this->leftRightTypeSettingHorizontal($i);
			if($re){
				if($useArea){
					if($re['typesettingNum']>$userNum){
							$useArea=$re['area'];
							$userNum=$re['typesettingNum'];
							$selectData=$re;
					}elseif($re['typesettingNum']==$userNum){
						// p($re);
						if($re['area']<$useArea){
							$useArea=$re['area'];
							$userNum=$re['typesettingNum'];
							$selectData=$re;
						}
					}			
				}else{
					$useArea=$re['area'];
					$userNum=$re['typesettingNum'];
					$selectData=$re;
				}
			}			
		}
		echo $selectData['description'];
		// p($selectData);
		return $selectData;		
	}
	// 上A下B排版
	public function upDownTypeSetting(){
		$paperWidth=$this->intPaper['width'];//纸宽度
		$paperHeight=$this->intPaper['height'];//纸高度
		$bleedingSiteW=$this->intProductBleedingSite['width'];//成品规格宽度（含出血位）
		$bleedingSiteH=$this->intProductBleedingSite['height'];	//成品规格高度（含出血位）
		$bitePosition=$this->intPrint['bitePosition'];//咬口位宽度
		$bleedingSite=$this->intPrint['bleedingSite'];//出血位宽度


		// 计算左右排，并且取出最优排
		$useArea=0;//使用面积
		$userNum=0;//使用数量
		$selectData=[];

		for($i=1;$i<=2;$i++){			
			// 上竖下横排版-下边行数 竖向
			$re=$this->upDownTypeSettingVertical($i);	
			if($re){				
				if($useArea){
					if($re['typesettingNum']>$userNum){
							$useArea=$re['area'];
							$userNum=intval($re['typesettingNum']);
							$selectData=$re;
					}elseif($re['typesettingNum']==$userNum){
						// p($re);
						if($re['area']<$useArea){
							$useArea=$re['area'];
							$userNum=intval($re['typesettingNum']);
							$selectData=$re;
						}
					}			
				}else{
					$useArea=$re['area'];
					$userNum=$re['typesettingNum'];
					$selectData=$re;
				}
			}			
		}
		// echo $selectData['description'];
		// p($userNum);
		for($i=1;$i<3;$i++){
			// 左横右竖排版-右边列数 横向
			$re=$this->upDownTypeSettingHorizontal($i);
			if($re){
				if($useArea){
					if($re['typesettingNum']>$userNum){
							$selectData=$re;
							$useArea=$re['area'];
							$userNum=intval($re['typesettingNum']);
					}
					if($re['typesettingNum']==$userNum){
						if($re['area']<$useArea){
							$selectData=$re;
							$useArea=$re['area'];
							$userNum=intval($re['typesettingNum']);
						}
					}			
				}else{
					$selectData=$re;
					$useArea=$re['area'];
					$userNum=$re['typesettingNum'];
				}
			}			
		}
		echo $selectData['description'];
		// p($selectData);
		return $selectData;			
	}

	// 显示已知信息
	public function showInformation(){
		///////////////////////////////////////////
		// '产品信息：长度285宽度210克重200数量100000纸吨价6000' //
		///////////////////////////////////////////
 
       $printingSurface=$this->intPrintProduct['printingSurface']==2?'双面':'单面';
        $printingContent=$this->intPrintProduct['printingContent']==2?'不同':'相同';

		echo "单张成品规格：{$this->intPrintProduct['width']} mm * {$this->intPrintProduct['height']} mm,数量：{$this->intPrintProduct['num']}张,双铜纸重量：{$this->intPrintProduct['weight']}g，印面：{$printingSurface}，正反内容：{$printingContent} 纸规格：{$this->intPaper['width']} mm * {$this->intPaper['height']}mm<br><br>";
	}


	public function getPrice(){

		// 显示已知信息
		$this->showInformation();
		//成品加出血位宽高
		$this->typesettingMethod();

	}


}


$w=210;//成品规格宽
$h=285;//成品规格高
$ww=200;//材料克重  
$num=100000;//成品数量  
$obj=new calcpaper();

$obj->setPro($h, $w, $ww, $num);
$obj->getPrice();
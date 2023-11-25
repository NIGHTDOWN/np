<?php


function p($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    // exit;
}
class Polygon
{
    private $vertices = [];
    private $points = [];
    //传入多边形各个点集合
    public function __construct($vertices)
    {
        $this->vertices = $vertices;

        //压入点数组
        $count = count($this->vertices) / 2;
        for ($i = 0; $i < $count; $i++) {

            array_push($this->points, array($this->vertices[$i * 2], $this->vertices[$i * 2 + 1]));
        }
    }
    public function proint($points)
    {
        $this->points = $points;
        foreach ($points as $key => $value) {
            # code...
            array_push($this->vertices, $value[0], $value[1]);
        }
    }
    // 计算多边形的面积  
    public function area()
    {
        $n = count($this->points);
        $area = 0;
        for ($i = 0; $i < $n; $i++) {
            $j = ($i + 1) % $n;
            $area += ($this->points[$j][0] - $this->points[$i][0]) * ($this->points[$i][1] + $this->points[$j][1]);
        }
        return abs($area / 2);
    }
}
class draw
{
    //纸规格列表
    // public $paperlist = [

    //     //大度全开
    //     [
    //         'paperWidth' => 1194,
    //         'paperHeight' => 889,
    //         'price' => 5000, //元每吨
    //         'paperName' => '大度全开纸'
    //     ],
    //     //大度对开
    //     [
    //         'paperWidth' => 889,
    //         'paperHeight' => 597,
    //         'price' => 5000, //元每吨
    //         'paperName' => '大度对开纸'
    //     ],
    //     //正度
    //     //正度全开
    //     [
    //         'paperWidth' => 1092,
    //         'paperHeight' => 787,
    //         'price' => 5000, //元每吨
    //         'paperName' => '正度全开纸'
    //     ],
    //     //正度对开
    //     [
    //         'paperWidth' => 787,
    //         'paperHeight' => 546,
    //         'price' => 5000, //元每吨
    //         'paperName' => '正度对开纸'
    //     ],
    //     //特规纸
    //     [
    //         'paperWidth' => 865,
    //         'paperHeight' => 595,
    //         'price' => 5000, //元每吨
    //         'paperName' => '特规纸'
    //     ],
    //     [
    //         'paperWidth' => 965,
    //         'paperHeight' => 635,
    //         'price' => 5000, //元每吨
    //         'paperName' => '特规纸'
    //     ],
    //     [
    //         'paperWidth' => 990,
    //         'paperHeight' => 710,
    //         'price' => 5000, //元每吨
    //         'paperName' => '特规纸'
    //     ],
    //     //分切纸
    //     [
    //         'paperWidth' => 1575,
    //         'paperHeight' => 0,
    //         'price' => 5000, //元每吨
    //         'paperName' => '分切纸'
    //     ],
    //     //分切纸
    //     [
    //         'paperWidth' => 1194,
    //         'paperHeight' => 0,
    //         'price' => 5000, //元每吨
    //         'paperName' => '分切纸'
    //     ],
    //     //分切纸
    //     [
    //         'paperWidth' => 1092,
    //         'paperHeight' => 0,
    //         'price' => 5000, //元每吨
    //         'paperName' => '分切纸'
    //     ],
    //     //分切纸
    //     [
    //         'paperWidth' => 889,
    //         'paperHeight' => 0,
    //         'price' => 5000, //元每吨
    //         'paperName' => '分切纸'
    //     ],
    //     //分切纸
    //     [
    //         'paperWidth' => 787,
    //         'paperHeight' => 0,
    //         'price' => 5000, //元每吨
    //         'paperName' => '分切纸'
    //     ],

    // ];
    public $intPrint = [
        'bleedingSite' => 3,
        'bitePosition' => 12
    ];

    // echo "出血位：{$intPrint['bleedingSite']}mm 咬口位：{$intPrint['bitePosition']}mm<br>";
    //成品要求
    // public $intPrintProduct = [
    //     'width' => 420, //成品规格-宽
    //     'height' => 285, //成品规格-高
    //     'num' => 100000, //成品数量10万
    //     'weight' => 200, //200克    
    //     'printingSurface' => 2, //印面 1-单面，2-双面
    //     'printingContent' => 2, //1-正反面内容相同，2-正反面内容不同
    // ];
    //     最小进纸规格
    // a、全开机：720*520mm； b、对开机：540*360mm
    // c、四开机：305*230mm； d、八开机：200*110mm*/
    // public $machine = [
    //     [
    //         'inPrintWidthMax' => 1400,
    //         'inPrintHeightMax' => 980,
    //         'inPrintWidthMin' => 720,
    //         'inPrintHeightMin' => 520,
    //         'num' => 0,
    //         'name' => '全开机'
    //     ],
    //     [
    //         'inPrintWidthMax' => 1020,
    //         'inPrintHeightMax' => 700,
    //         'inPrintWidthMin' => 540,
    //         'inPrintHeightMin' => 360,
    //         'num' => 2,
    //         'name' => '对开机'
    //     ],
    //     [
    //         'inPrintWidthMax' => 720,
    //         'inPrintHeightMax' => 500,
    //         'inPrintWidthMin' => 305,
    //         'inPrintHeightMin' => 230,
    //         'num' => 4,
    //         'name' => '四开机'
    //     ],
    //     [
    //         'inPrintWidthMax' => 510,
    //         'inPrintHeightMax' => 340,
    //         'inPrintWidthMin' => 200,
    //         'inPrintHeightMin' => 110,
    //         'num' => 8,
    //         'name' => '八开机'
    //     ],
    // ];
    //排版
    /**
     * $pgw     纸宽度
     * $pgh     纸高度
     * $pw      产品宽度
     * $ph      产品高度
     * $hnum 横向个数
     * $snum 竖向个数
     */
    public function draw($pgw, $pgh, $pw, $ph, $hnum, $snum)
    {

        $image = imagecreatetruecolor($pgw, $pgh);

        // 设置矩形颜色  
        // // $rectColor = Imaging_Resource::COLOR_WHITE;
        for ($i = 0; $i < $hnum; $i++) {
            # code...
            for ($i1 = 0; $i1 < $snum; $i1++) {
                # code...
                $this->_draw($image, $pw, $ph, $i, $i1);
            }
        }
        $this->_drawSeam($image, 1, $pgw, $pgh);

        // 创建图像  




        $this->_drawnum($image, [0, 0, $pgw, 0, $pgw, $pgh, 0, $pgh], 30, 0);
        // 保存图像  
        $str = "pic/" . rand(111, 999999) . $hnum . "-" . $snum . ".png";
        imagepng($image, $str);
        // 释放内存  
        imagedestroy($image);


        return $str;
    }
    /**
     * 绘制矩形边数字
     */
    private function _drawnum($image, $point, $fontsize = null, $color = null)
    {
        $font_path = 'D:\phpstudy\WWW\c.com\font.ttf'; // 字体路径
        $font_size = $fontsize ?? 16;
        $text_color = $color ?? imagecolorallocate($image, 255, 255, 255);
        //标注长
        $x = ($point[2] - $point[0]) / 2 + $point[0];
        $y = 0 + $font_size + $point[1];
        $text = $point[2] - $point[0];
        imagettftext($image, $font_size, 0, $x, $y, $text_color, $font_path, $text);
        // p("$x,$y");
        //标注宽
        $y = ($point[5] - $point[1]) / 2 + $point[1];
        $x = 0 + $point[0];
        $text = $point[5] - $point[1];
        imagettftext($image, $font_size, 0, $x, $y, $text_color, $font_path, $text);
    }
    private $borderColors = [0xf50606, 0xf5ef09];
    private $bordercolorindex = 0;
    //画板在固定位置画矩形
    private function _draw($canvasobj, $pw, $ph, $hnum, $snum)
    {
        $this->bordercolorindex = $this->bordercolorindex ? 0 : 1;
        //红色出血
        $color = $this->borderColors[$this->bordercolorindex];
        $x1 = $hnum * $pw;
        $y1 = $snum * $ph;
        imagerectangle($canvasobj, $x1, $y1, $pw, $ph, $color);
        $points = [
            $x1, $y1,
            $x1 + $pw, $y1,
            $x1 + $pw, $y1 + $ph,
            $x1, $y1 + $ph,
        ];
        // imagefilledpolygon($canvasobj,$points,4 $color);
        imagefilledpolygon($canvasobj, $points, 4, $color);
        $this->_drawpro($canvasobj, $pw, $ph, $hnum, $snum);
        $this->_drawnum($canvasobj, $points);
    }
    //绘制产品
    private function _drawpro($canvasobj, $pw, $ph, $hnum, $snum)
    {
        $color = rand(0, 999999);
        // $color = 0xffffff;

        $w = $this->intPrint['bleedingSite'];
        $x1 = $hnum * $pw + $w;
        $y1 = $snum * $ph + $w;
        $pw = $pw - ($w * 2);
        $ph = $ph - ($w * 2);
        $points = [
            $x1, $y1,
            $x1 + $pw, $y1,
            $x1 + $pw, $y1 + $ph,
            $x1, $y1 + $ph,
        ];
        if ($_POST['prourl']) {
            $this->_drawimg($canvasobj, $points, $_POST['prourl']);
        } else {
            imagefilledpolygon($canvasobj, $points, 4, $color);
        }
        // imagefilledpolygon($canvasobj,$points,4 $color);

    }
    /**
     * 画咬口
     * 咬口数量
     * $num 咬口数量
     * $x 大纸宽度
     * ￥y 大纸高度
     */
    public function _drawSeam($canvasobj, $num, $x, $y)
    {
        $color = 0xf50606;;
        $w = $this->intPrint['bitePosition'] - $this->intPrint['bleedingSite'];
        if ($num == 1) {

            $points = [
                0, $y - $w,
                $x, $y - $w,
                $x, $y,
                0, $y,
            ];
            // imagefilledpolygon($canvasobj,$points,4 $color);
            imagefilledpolygon($canvasobj, $points, 4, $color);
        }
    }
    private $imgobj;
    public function _drawimg($canvasobj, $point, $img)
    {

        // 创建图像  
        $image = $canvasobj;

        // 加载图片  
        // $logo_path = 'path/to/logo.png';
        $logo_path = $img;
        $logo_width = $point[2] - $point[0];
        $logo_height =  $point[5] - $point[1];
        $logo_x = $point[0];
        $logo_y = $point[1];

        $logo_image = $this->imgobj = $this->imgobj ?? imagecreatefromjpeg($logo_path);
        // 调整贴图大小  
        $new_width = $logo_width;
        $new_height = $logo_height;
        list($width, $height) = getimagesize($logo_path);
        if ($width > $height) {
            $new_height = $new_width * $height / $width;
        } else {
            $new_width = $new_height * $width / $height;
        }

        // 调整贴图位置  
        $logo_x_offset = ($new_width - $logo_width) / 2;
        $logo_y_offset = ($new_height - $logo_height) / 2;
        // 粘贴贴图  
        imagecopyresampled($image, $logo_image, $logo_x + $logo_x_offset, $logo_y + $logo_y_offset, 0, 0, $new_width, $new_height, $width, $height);
        // imagedestroy($logo_image);
    }
    //检查B跟C区域剩余空间是否可以排
    private function checkCandB($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY)
    {
        $num = $typesettingX * $typesettingY;
        $totalmj = $paperWidth * $paperHeight;
        $promj = $typesettingW * $typesettingH;
        $prototalmj = $promj * $num;
        if (($totalmj - $prototalmj) / $promj > 1) {
            $this->checkC($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY);
            $this->checkB($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY);
        }
    }
    //检查C区域
    private function checkC($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY)
    {
        $num = ($paperWidth - ($typesettingW * $typesettingX)) / $typesettingH;
        $num2 = $paperHeight / $typesettingW;
        $num = floor($num);
        $num2 = floor($num2);
        $num3=$num*$num2;
        if ($num3 > 1) {
            p("B区域可以排".$num3);
        }
    }
    //检查B区域
    private function checkB($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY)
    {
        $num = ($paperHeight - ($typesettingH * $typesettingY)) / $typesettingW;
        $num2 = $paperWidth / $typesettingW;
        $num = floor($num);
        $num2 = floor($num2);
        $num3=$num*$num2;
        if ($num3 > 1) {
            p("c区域可以排".$num3 );
        }
    }
    //使用纸具体计价
    public function getPriceByPaper($intPrint, $intPrintProduct, $intPaper, $machine = [])
    {

        $paperWidth = $intPaper['paperWidth'];
        $paperHeight = $intPaper['paperHeight'];
        $price = $intPaper['price'];

        if ($machine) {
            echo '<br>使用<b style="color:red">' . $intPaper['paperName'] . '</b>排版,规格：宽：' . $paperWidth . 'mm 高：' . $paperHeight . 'mm 每吨' . $price . '元<br>';
            echo "机器名：{$machine['name']} 最大进宽：{$machine['inPrintWidthMax']}mm 高：{$machine['inPrintHeightMax']}mm<br>";
        } else {
            echo '<br>使用<b>' . $intPaper['paperName'] . '</b>排版,规格：宽：' . $paperWidth . 'mm 高：' . $paperHeight . 'mm 每吨' . $price . '元<br>';
        }
        // var_dump($intPrintProduct);
        // p($intPrintProduct);
        $bleedingSiteW = $intPrintProduct['width'] + $intPrint['bleedingSite'] * 2;
        $bleedingSiteH = $intPrintProduct['height'] + $intPrint['bleedingSite'] * 2;
        // p( $paperWidth);
        // p( $bleedingSiteW);
        // p( $intPrintProduct);
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

        $typesettingShape = ''; //排版形状
        if ($tmpNum >= $tmpNum2) {
            //横排
            $typesettingX = $tmpW;
            $typesettingY = $tmpH;
            $typesettingNum = $tmpNum;
            $typesettingW = $bleedingSiteW;
            $typesettingH = $bleedingSiteH;

            $typesettingShape = $typesettingW >= $typesettingH ? '横排' : '竖排';
        } else {
            //竖排
            $typesettingX = $tmpW2;
            $typesettingY = $tmpH2;
            $typesettingNum = $tmpNum2;
            $typesettingW = $bleedingSiteH;
            $typesettingH = $bleedingSiteW;
            $typesettingShape = $typesettingW >= $typesettingH ? '横排' : '竖排';
        }

        echo "成品{$typesettingShape}排版 宽：{$typesettingW}mm 高：{$typesettingH}mm(含出血位)<br>";

        echo '横排数量:' . $typesettingX . '个 纵排数量:' . $typesettingY . '个 排版总数:' . $typesettingNum . '个<br>';
        //判断数量奇偶
        $img = $this->draw($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY);
        $this->checkCandB($paperWidth, $paperHeight, $typesettingW, $typesettingH, $typesettingX, $typesettingY);
        $unfoldW = $typesettingW * $typesettingX;
        $unfoldH = $typesettingH * $typesettingY + $intPrint['bitePosition'] - $intPrint['bleedingSite'];

        //    $paperWidth=$paperWidthJs;
        //    $paperHeight=$paperHeightJs;

        if (($paperWidth >= $unfoldW && $paperHeight >= $unfoldH) || ($paperWidth >= $unfoldH && $paperHeight >= $unfoldW)) {
            echo "展开面积规格（含1个咬口）:宽：{$unfoldW}mm 高：{$unfoldH}mm<br>";
            if (is_int($typesettingX / 2)) {
                echo '横排数量偶数，适合左右自反版<br>';
            } else {
                echo '横排数量奇数，适合正反版<br>';
            }
        } else {
            echo "展开面积规格加上1个咬口后面积超出，需要减少排版数量<br>";
        }

        //单面不考虑双咬口
        /*$unfoldH2=$typesettingH*$typesettingY+$intPrint['bitePosition']*2-$bleedingSite*2;
    if(($paperWidth>=$unfoldW && $paperHeight>=$unfoldH2) || ($paperWidth>=$unfoldH2 && $paperHeight>=$unfoldW) ){
        echo "展开面积规格（含2个咬口）:宽：{$unfoldW}mm 高：{$unfoldH2}mm<br>";
    }else{

    } */

        //价格
        $needNum = ceil($intPrintProduct['num'] / $typesettingNum);

        //需要对开纸数量
        echo "需要{$intPaper['paperName']}数量:{$needNum}张<br>";
        $price = $_POST['price'] ?? $intPaper['price'];
        //对开纸一张价格
        $needPrice = $price / 1000000 * $intPrintProduct['weight'] * $paperWidth * $paperHeight * 0.001 * 0.001;
        echo $intPaper['paperName'] . "一张价格:  " . $needPrice . '元<br>';
        $paperPrice = $needNum * $needPrice;
        echo "{$needNum}张{$intPaper['paperName']}价格:  " . $paperPrice . '元<br>';
        global $lowestPrice;

        if ($lowestPrice) {
            $lowestPrice = $lowestPrice > $paperPrice ? $paperPrice : $lowestPrice;
        } else {
            $lowestPrice = $paperPrice;
        }
        echo '  <script src="http://www.ng169.com/tpl/static/js/encode/jquery.min.js" type="text/javascript"></script>
        <script src="http://www.ng169.com/tpl/static/js/night_Trad.v1.0.js" type="text/javascript"></script>';
        echo "<img style='width:250px' onclick='_go_url_new(\"" . $img . "\")' src='" . $img . "'>";
        /*//价格最优
    $lowestPrice=0;
    //车头数最少
    $PrintingTimes=0;*/
    }

    //使用纸计价
    /**
     * $intPrint 出血位信息
     * $intPrintProduct 成品信息
     * $intPaper 纸信息
     * $machine  机器
     */
    public function usePaper($intPrint, $intPrintProduct, $intPaper, $machine = [])
    {
        $paperWidth = $intPaper['paperWidth'];
        $paperHeightOld = $paperHeight = $intPaper['paperHeight'];
        $price = $intPaper['price']; //元每吨

        if (!$paperWidth || !$paperHeight) {
            //        p($machine);
            // if ($machine) {
            // foreach ($machine as $k => $v) {
            // p(json_encode($v));
            if ($machine['inPrintWidthMax'] >= $paperWidth) {
                $paperHeight = $machine['inPrintHeightMax'];
                //                    var_dump([$paperWidth,$paperHeight,$v['name']]);
                //                    
                $intPaper['paperHeightOld'] = $paperHeightOld;
                $intPaper['paperWidth'] = $paperWidth;
                $intPaper['paperHeight'] = $paperHeight;
                //                    p($intPaper);
                $this->getPriceByPaper($intPrint, $intPrintProduct, $intPaper, $machine);
            }
            // }
            // }
            //        p('end');
        } else {
            //        p([$intPrint,$intPrintProduct,$intPaper,$machine]);
            $this->getPriceByPaper($intPrint, $intPrintProduct, $intPaper, $machine);
        }
    }
    //生成机器组合
    private $doloop = [];
    private function addgroup($machineid, $paperid)
    {
        //检查组合搭配；
        array_push($this->doloop, [$machineid, $paperid, "desc" => ""]);
    }
    public function calc()
    {
        //生产组合

        foreach ($this->machine as $key => $m) {
            # code...
            foreach ($this->paperlist as $pk => $p) {
                # code...
                $this->addgroup($key, $pk);
                // p("$key,$pk");
            }
        }
        // p($this->doloop);
        $this->_calc();
    }
    //线程或者携程执行组合运算
    public function _calc()
    {
        foreach ($this->doloop as $key => $g) {
            # code...
            $this->usePaper($this->intPrint, $this->intPrintProduct, $this->paperlist[$g[1]], $this->machine[$g[0]]);
        }
    }
    public function setPro($h, $w, $ww, $num)
    {
        $this->intPrintProduct['height'] = $h;
        $this->intPrintProduct['width'] = $w;
        $this->intPrintProduct['weight'] = $ww;
        $this->intPrintProduct['num'] = $num;
    }
}


// (new calcpaper())->calc();

if ($_POST['height']) {
    // p(22222);
    p("产品信息：长度" . $_POST['height'] . "宽度" . $_POST['width'] . "克重" . $_POST['weight'] . "数量" . $_POST['num'] . "纸吨价" . $_POST['price']);
    $obj = new calcpaper();
    $obj->setPro($_POST['height'], $_POST['width'], $_POST['weight'], $_POST['num']);
    $obj->calc();
}

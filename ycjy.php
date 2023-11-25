<?php  
header('Content-Type: text/html; charset=utf-8');  
  
// 定义问题  
$targetFunction = 'maximize(f($x$,$y$))'; // 目标函数，这里假设为最大化 f(x,y)  
$xMax = 50; // x 轴最大值  
$yMax = 50; // y 轴最大值  
$numPolygons = 8; // 多边形数量  
$polygonAreaMax = 300; // 多边形面积最大值  
  
// 遗传算法参数  
$populationSize = 50; // 种群大小  
$numGenerations = 100; // 迭代次数  
$crossoverProb = 0.8; // 交叉概率  
$mutationProb = 0.1; // 变异概率  
  
// 编码  
$genotype = '';  
for ($i = 0; $i < $numPolygons; $i++) {  
    $polygon = getRandomPolygon($polygonAreaMax); // 随机生成多边形  
    $polygonPoints = getPolygonPoints($polygon); // 转换为点集  
    $polygonEncoding = getEncoding($polygonPoints); // 对多边形进行编码  
    $genotype .= $polygonEncoding; // 将多边形编码拼接到基因型中  
}  
  
// 初始化种群  
$population = [];  
for ($i = 0; $i < $populationSize; $i++) {  
    $newGenotype = '';  
    for ($j = 0; $j < strlen($genotype); $j++) {  
        $newGenotype .= $genotype[$j] == '0' ? '1' : '0'; // 随机生成基因型  
    }  
    $population[] = $newGenotype;  
}  
  
// 评估适应度  
$fitnessValues = [];  
foreach ($population as $genotype) {  
    $polygonSet = getPolygonsFromEncoding($genotype); // 将基因型解码为多边形集合  
    $area = getPolygonArea($polygonSet); // 计算多边形集合的总面积  
    $fitnessValue = $targetFunction == 'maximize(f($x$,$y$))' ? $area : -$area; // 判断目标函数类型，最大化或最小化  
    $fitnessValues[] = $fitnessValue;  
}  
  
// 选择个体  
$selectedIndividuals = [];  
$rouletteWheel = []; // 轮盘赌算法中的累加概率数组  
foreach ($fitnessValues as $fitnessValue) {  
    if (!isset($rouletteWheel[$fitnessValue])) {  
        $rouletteWheel[$fitnessValue] = 0;  
    }  
    $rouletteWheel[$fitnessValue] += 1; // 累加概率  
}  
$rouletteWheelTotal = array_sum($roulettewheel); // 总概率

$selectedIndividuals=[];
for($i = 0; $i<$populationSize; $i++) 
{ 
    $rand = rand(1, $rouletteWheelTotal);
    $sum = 0;
    foreach ($rouletteWheelasvalue) {
        $sum+=$value;
    if ($sum>=$rand) {
        $selectedIndividuals[]=$i; // 选中个体
    break;
    }
    }
    }
    
    // 交叉操作
    $offspring=[];for($i = 0; $i<$populationSize; $i++) { $parent1 = $selectedIndividuals[rand(0,count($selectedIndividuals) - 1)];
        $parent2=$selectedIndividuals[rand(0, count($selectedIndividuals)−1)];if(rand(1,100)<=$crossoverProb || i == 0) { $offspring[] = crossover($population[$parent1], $population[$parent2]);
    } else {
        $offspring[]=$population[$parent1];
    }
    }
    
    // 变异操作
    for ($i=0;$i < count($offspring);$i++) {
    if (rand(1, 100) <= $mutationProb) { $offspring[i]=mutate($offspring[$i]);
    }
    }
    
    // 生成下一代种群
    $population=$offspring;
    
    // 检查停止条件
    if ($generation==$numGenerations) {
    break;
    }
    
    // 返回最终种群中适应度最高的个体
    $bestIndividual=$population[0];
    $polygonSet=getPolygonsFromEncoding($bestIndividual);
    $area=getPolygonArea($polygonSet);
    echo 'The maximum area for '.$numPolygons. 
 
    $polygonsis 
   
     .$area.'<br>';
    
    // 辅助函数
    
    function getRandomPolygon($maxArea) { 
        $points = [];
        $sides=rand(3,6);
        $area = 0;
    while ($area<$maxArea) {
        $x=rand(−$maxArea, $maxArea);
        $y = rand(-$maxArea,$maxArea);
        $points[]=[$x, $y];
        $area += abs($x)∗abs($y);
    }
    shuffle($points);return $points;
    }
    
    function getPolygonPoints($polygon) { 
        $points = '';
    foreach ($polygon as $point) {
        $points .= ' '.round($point[0]).','.round($point[1]).' '; 
    } return $points;
    }
    
    function getEncoding($points) { $encoding = '';
    for ($i=0;$i < strlen($points);$i += 2) {
        $x=intval(substr($points, $i,2));$encoding .= ($x & 1) == 0 ? '0' : '1'; } return $encoding;
    }
    
    function getPolygonsFromEncoding($encoding) { 
        $polygons = [];
        $numPolygons=strlen($encoding) / 4; // 每四个字符表示一个多边形
    for ($i=0;$i < $numPolygons;$i++) {
        $polygon=[];
        for($j = 0; $j<4;$j++) {
            $k=4∗$i + $j;
            $bit = intval($encoding[$k]);
            $x=($bit << (2 * ($jy = ($bit>>(2∗(1−$j % 2))) & 0xff;
            $polygon[]=[$x, $y]; 
        }
         sortByDistance($polygon[0], $polygon);//将多边形的第一个点作为中心点排序polygons[] = polygon; } return polygons;
    }
    
    function crossover($parent1,$parent2) {
        $length=strlen($parent1);
    $crossoverPoint1=rand(0,$length - 4); // 随机交叉点1
    $crossoverPoint2=rand($crossoverPoint1 + 4, $length);//随机交叉点2，确保不重复交叉点1和字符串结尾
    $child = '';
    for ($i=0;$i < $crossoverPoint1;$i++) {
        $child.=$parent1[$i]; } 
        for (i = $crossoverPoint1;$i < $crossoverPoint2;$i++) {
        $child.=$parent2[$i];
     } 
        for (i = $crossoverPoint2;$i < $length;$i++) {
        $child.=$parent1[$i]; 
    } 
    return child;
        }
        
        function mutate($gene) { 
            $length = strlen($gene);
            $mutationPoint1 = rand(0, $length−4);//随机变异点1mutationPoint2 = rand(mutationPoint1+4,length); // 随机变异点2，确保不重复变异点1和字符串结尾
            $mask="";
         for($i = 0; $i<$mutationPoint1; $i++) { 
            $mask .= '0';
        }
        for ($i=$mutationPoint1; $i<$mutationPoint2; $i++) {
             $mask .= '1';
        }
        for ($i=$mutationPoint2; $i<$length; $i++) { 
            $mask .= '0';
        }
        $child="";
        for($i = 0; i<$length; $i++) {
             $child .= ($gene[$i] == '0' && $mask[$i] == '1') ? '1' : ($gene[$i] == '1' && $mask[$i] == '0' ? '0' : $gene[$i]);
        }
        return $child;
        }
        
        function sortByDistance($point, &$polygon) {
            $prev=$polygon[count($polygon)−1];
            foreach($polygon as $next) { 
                $distance = abs($point[0]−$next[0]) + abs($point[1]−$next[1]);
        if ($distance > 0) { 
            usort($polygon, function ($a,$b) use ($distance,$point) {

            $distA=abs(point[0] - a[0])+abs($point[1] - $a[1]);
            $distB = abs($point[0]−$b[0]) + abs($point[1]−$b[1]);
        if ($distA==$distB) {
        return 0;
        }
        return ($distA>$distB) ? 1 : -1;
        });
        break;
        }
        }
        }
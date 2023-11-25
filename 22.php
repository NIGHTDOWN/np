<?php
// 多边形类  
class Polygon
{
    public $points;
    public $area;
    private $vertices = [];
    public function __construct($points)
    {
        $this->points = $points;
        $this->area = calculateArea($this->points);
    }
    public function draw($image)
    {
        // $image = imagecreatetruecolor(1000, 1000);
        // $text_color = $color ?? imagecolorallocate($image, 255, 255, 255);
        $text_color=rand(0,999999);
        imagefilledpolygon($image, $this->vertices(),(count($this->points))/2, $text_color);
    }
    public function vertices()
    {
       
        foreach ($this->points as $key => $value) {
            # code...
            array_push($this->vertices, $value[0], $value[1]);
        }
        return $this->vertices;
    }
    
}
class image
{
    private $image;
    public function __construct($x, $y)
    {
        $this->image = imagecreatetruecolor($x, $y);
    }
    public function save()
    {
        $str = "pic/" . rand(111, 999999) . ".png";
        imagepng($this->image, $str);
        // 释放内存  
        imagedestroy($this->image);
        return $str;
    }
    public function getres()
    {
        return $this->image;
    }
}
function p($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    // exit;
}

// 多边形的规格  
$specifications = [  
    [4, 4], // 第一个多边形规格，边长为4，数量无限  
    [6, 6], // 第二个多边形规格，边长为6，数量无限  
    [8, 8]  // 第三个多边形规格，边长为8，数量无限  
];  
  
// 纸张大小  
$paperSize = [  
    'width' => 100, // 纸张宽度  
    'height' => 100 // 纸张高度  
];  
  
// 种群数量  
$populationSize = 100;  
  
// 迭代次数  
$generationCount = 100;  
  
// 遗传挤压概率  
$squeezeChance = 0.2;  
  
// 交叉概率  
$crossoverChance = 0.8;  
  
// 变异概率  
$mutationChance = 0.1;  
  
// 存储每一代的最优解  
$bestIndividuals = array();  
  
// 开始迭代  
for ($i = 0; $i < $generationCount; $i++) {  
    // 初始化种群  
    $individuals = array();  
    for ($j = 0; $j < $populationSize; $j++) {  
        $individual = array();  
        foreach ($specifications as $spec) {  
            $x = rand(0, $paperSize['width'] - $spec[0]);  
            $y = rand(0, $paperSize['height'] - $spec[1]);  
            $individual[] = array($x, $y);  
        }  
        $individuals[] = $individual;  
    }  
      
    // 计算每个个体的适应度  
    foreach ($individuals as $index => $individual) {  
        $fitness = calculateFitness($individual, $specifications, $paperSize);  
        $individuals[$index]['fitness'] = $fitness;  
    }  
      
    // 选择最优解  
    $bestIndex = 0;  
    $bestFitness = $individuals[0]['fitness'];  
    foreach ($individuals as $index => $individual) {  
        if ($individual['fitness'] > $bestFitness) {  
            $bestFitness = $individual['fitness'];  
            $bestIndex = $index;  
        }  
    }  
    $bestIndividual = $individuals[$bestIndex];  
    $bestIndividuals[] = $bestIndividual;  
      
    // 选择父代进行遗传操作  
    $parents = array();  
    for ($j = 0; $j < $populationSize; $j++) {  
        $parent1 = rand(0, $populationSize);  
        $parent2 = rand(0, $populationSize);  
        while ($parent1 == $parent2) {  
            $parent2 = rand(0, $populationSize);  
        }  
        $parent1Points = $individuals[$parent1];  
        $parent2Points = $individuals[$parent2];  
        $parents[] = crossover($parent1Points, $parent2Points, $crossoverChance);  
        if (rand() / count($parents) < $mutationChance) {  
            $parents[rand(0, count($parents) - 1)][] = mutation($parents[rand(0, count($parents) - 1)], $specifications, $paperSize);  
        }  
    }  
      
    // 选择子代进行遗传操作  
    $children = array();  
    for ($j = 0; $j < $populationSize; $j++) {  
        $child = clone($parents[rand(0, count($parents) - 1)]);  
        if (rand() / count($parents) < $crossoverChance) {  
            $child = crossover($child, $parents[rand(0, count($parents) - 1)], $crossoverChance);  
        }  
        if (rand() / count($parents) < $mutationChance) {  
            $child[] = mutation($child, $specifications, $paperSize);  
        }  
        $children[] = $child;  
    }  
      
    // 更新种群  
    $individuals = $children;  
}  
function crossover($parent1, $parent2, $crossoverChance) {  
    $child = array();  
    $crossoverPoint = rand(0, count($parent1));  
    for ($i = 0; $i < $crossoverPoint; $i++) {  
        $child[] = $parent1[$i];  
    }  
    for ($i = $crossoverPoint; $i < count($parent2); $i++) {  
        $child[] = $parent2[$i];  
    }  
    return $child;  
}  
function calculateFitness($individual, $polygons, $targetArea)
{
    $fitness = 0;
    foreach ($individual as $point) {
        $fitness += $point[0] * $point[1];
    }
    $fitness /= count($individual);
    foreach ($polygons as $polygon) {
        $polygonArea = $polygon->area;
        $x = $polygon->points[0][0];
        $y = $polygon->points[0][1];
        foreach ($individual as $point) {
            if (
                $point[0] <= $x + $polygonArea &&
                $point[0] >= $x &&
                $point[1] <= $y + $polygonArea &&
                $point[1] >= $y
            ) {
                $fitness -= 1;
                break;
            }
        }
    }
    return $fitness;
}
  // 变异函数  
function mutation($individual, $specifications, $paperSize) {  
    $mutationPoint = rand(0, count($individual) - 1);  
    $x = $individual[$mutationPoint][0] + rand(-5, 5);  
    $y = $individual[$mutationPoint][1] + rand(-5, 5);  
    while ($x > $paperSize['width'] || $x < 0 || $y > $paperSize['height'] || $y < 0) {  
        $x = $individual[$mutationPoint][0] + rand(-5, 5);  
        $y = $individual[$mutationPoint][1] + rand(-5, 5);  
    }  
    $individual[$mutationPoint][0] = $x;  
    $individual[$mutationPoint][1] = $y;  
    return $individual;  
}
// 输出现有的最优解的点在纸内的坐标值，每个多边形以逗号分隔，作为数组返回。每个多边形的点坐标按照顺时针顺序排列。
// 输出现有的最优解的点在纸内的坐标值  
$bestIndividualPoints = array();  
foreach ($bestIndividual as $index => $individual) {  
    $polygonPoints = array();  
    for ($i = 0; $i < count($individual); $i++) {  
        $x = $individual[$i][0];  
        $y = $individual[$i][1];  
        $polygonPoints[] = "(" . $x . ", " . $y . ")";  
    }  
    $bestIndividualPoints[] = "[" . implode(", ", $polygonPoints) . "]";  
}  
echo "Best individual points: " . implode(", ", $bestIndividualPoints) . "\n";  
  
// 输出每个多边形的点在平面内的坐标  
foreach ($bestIndividual as $index => $individual) {  
    $polygonPoints = array();  
    for ($i = 0; $i < count($individual); $i++) {  
        $x = $individual[$i][0];  
        $y = $individual[$i][1];  
        $polygonPoints[] = "(" . $x . ", " . $y . ")";  
    }  
    echo "Polygon " . ($index + 1) . ": ";  
    echo implode(" ", $polygonPoints);  
    echo "\n";  
}  
?>
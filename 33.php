<?php  
function p($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    // exit;
}

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
// 多边形的规格和数量  
$specifications = [  
    [3, 5], // 三角形，数量5  
    [4, 10], // 四边形，数量10  
    [5, 20] // 五边形，数量20  
];  
  
// 大度纸的尺寸  
$paperSize = [  
    'width' => 100,  
    'height' => 100  
];  
  
// 种群数量和迭代次数  
$populationSize = 50;  
$generationCount = 100;  
  
// 随机数种子  
srand(time());  
  
// 初始化种群  
$population = [];  
foreach ($specifications as $spec) {  
    $count = $spec[1];  
    $polygons = [];  
    for ($i = 0; $i < $count; $i++) {  
        $polygon = [];  
        for ($j = 0; $j < $spec[0]; $j++) {  
            $x = rand(0, $paperSize['width']);  
            $y = rand(0, $paperSize['height']);  
            $polygon[] = [$x, $y];  
        }  
        $polygons[] = $polygon;  
    }  
    $population[] = $polygons;  
}  
  
// 遗传挤压算法  
for ($i = 0; $i < $generationCount; $i++) {  
    // 计算适应度  
    foreach ($population as &$polygon) {  
        $area = calculatePolygonArea($polygon, $paperSize);  
        $polygon['fitness'] = $area / ($paperSize['width'] * $paperSize['height']);  
    }  
    // 选择父代  
    $parents = [];  
    foreach ($population as $polygon) {  
        if ($polygon['fitness'] > 0) {  
            $parents[] = $polygon;  
        }  
    }  
    if (count($parents) < $populationSize) {  
        // 如果父代数量不足，则进行交叉和变异操作生成新的子代  
        $newPopulation = [];  
        while (count($newPopulation) < $populationSize) {  
            $parent1 = rand(0, count($parents) - 1);  
            $parent2 = rand(0, count($parents) - 1);  
            while ($parent1 == $parent2) {  
                $parent2 = rand(0, count($parents) - 1);  
            }  
            $child = crossover($parents[$parent1], $parents[$parent2]);  
            foreach ($child as &$subPolygon) {  
                foreach ($subPolygon as &$point) {  
                    $point[0] += rand(-5, 5);  
                    $point[1] += rand(-5, 5);  
                }  
            }  
            $newPopulation[] = $child;  
        }  
        $population = $newPopulation;  
    } else {  
        // 如果父代数量足够，则直接选择适应度高的父代作为下一代的父代  
        $parents = shuffleParents($parents);  
    }  
}  
  
// 输出结果  
echo "总共放置了 " . count($population) . " 个多边形\n";  
$totalArea = 0;  
foreach ($population as $polygon) {  
    $area = calculatePolygonArea($polygon, $paperSize);  
    $totalArea += $area;  
    // echo "多边形 " . count($polygon) . ": " . $area . " 平方单位\n";  
}  
// echo "总共占用了 " . $totalArea . " 平方单位面积\n";  
  
// 输出每个规格的多边形总数  
$specCount = [];  
foreach ($specifications as $spec) {  
    $count = 0;  
    foreach ($population as $polygon) {  
        if (count($polygon) == $spec[0]) {  
            $count++;  
        }  
    }  
    $specCount[$spec[0]] = $count;  
    echo "规格为 " . $spec[0] . " 的多边形总数为 " . $count . "\n";  
}  
  
// 输出每个多边形的点坐标  
foreach ($population as $polygon) {  
    echo "多边形点坐标:\n";  
   
    foreach ($polygon as $point) {  
        // echo "(" . $point[0] . ", " . $point[1] . ")\n";  
        p($point);
    }  
}


// 计算多边形的面积  
function calculatePolygonArea($polygon, $paperSize) {  
    $area = 0;  
    foreach ($polygon as $subPolygon) {  
        $x1 = $subPolygon[0][0];  
        $y1 = $subPolygon[0][1];  
        $x2 = $subPolygon[1][0];  
        $y2 = $subPolygon[1][1];  
        $x3 = $subPolygon[2][0];  
        $y3 = $subPolygon[2][1];  
        $area += ($x1 * $y2 + $x2 * $y3 + $x3 * $y1 - $x3 * $y2 - $x1 * $y3 - $x2 * $y1);  
    }  
    return $area / 2;  
}  
  
// 打乱父代的顺序  
function shuffleParents($parents) {  
    $shuffledParents = [];  
    while (count($parents) > 0) {  
        $index1 = rand(0, count($parents) - 1);  
        $index2 = rand(0, count($parents) - 1);  
        while ($index1 == $index2) {  
            $index2 = rand(0, count($parents) - 1);  
        }  
        $shuffledParents[] = $parents[$index1];  
        array_splice($parents, $index1, 1);  
    }  
    return $shuffledParents;  
}  
  
// 进行交叉操作  
function crossover($parent1, $parent2) {  
    $child = [[], [], []];  
    $crossoverPoint1 = rand(0, count($parent1) - 2);  
    $crossoverPoint2 = rand($crossoverPoint1 + 1, count($parent1) - 1);  
    $child[0] = array_slice($parent1[0], $crossoverPoint1, ($crossoverPoint2 - $crossoverPoint1 + 1));  
    $child[1] = array_slice($parent1[1], $crossoverPoint1, ($crossoverPoint2 - $crossoverPoint1 + 1));  
    $child[2] = array_slice($parent2[0], $crossoverPoint1, ($crossoverPoint2 - $crossoverPoint1 + 1));  
    return $child;  
}
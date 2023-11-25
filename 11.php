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

// 计算多边形面积  
function calculateArea($points)
{
    $area = 0;
    for ($i = 0; $i < count($points) - 1; $i++) {
        $area += ($points[$i][0] * $points[$i + 1][1] - $points[$i + 1][0] * $points[$i][1]);
    }
    $area /= 2;
    return abs($area);
}

// 生成随机多边形  
function generatePolygons($num, $maxArea)
{
    $polygons = [];
    while (count($polygons) < $num) {
        $points = [];
        $sideLength = rand(1, $maxArea / 2);
        for ($i = 0; $i < 3; $i++) {
            $x = rand(-20, 20);
            $y = rand(-20, 20);
            $points[] = [$x, $y];
        }
        $polygon = new Polygon($points);
        if ($polygon->area <= $maxArea) {
            $polygons[] = $polygon;
        }
    }
    return $polygons;
}

// 这个实现计算了多边形的适应度，即每个多边形在平面内的适应度之和。适应度越高，多边形在平面内的布局越合理。计算适应度时，它将每个点的 x 和 y 坐标相乘，然后将其除以多边形的数量。然后，对于每个多边形，它检查平面内的每个点是否在该多边形内部。如果是，适应度将减去 1。最后，返回适应度的总和。

// 请注意，这只是一个简单的实现示例，您可以根据您的具体需求进行更改和优化

// 计算多边形的适应度  
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
// 遗传挤压算法  
// function geneticSqueeze($polygons, $targetArea)
// {
//     $populationSize = 100;
//     $generationCount = 100;
//     $eliteCount = 10;
//     $mutationChance = 0.1;

//     $population = [];
//     for ($i = 0; $i < $populationSize; $i++) {
//         $individual = [];
//         foreach ($polygons as $polygon) {
//             $x = rand(0, $targetArea - $polygon->area);
//             $y = rand(0, $targetArea - $polygon->area);
//             $individual[] = [$x, $y];
//         }
//         $population[] = $individual;
//     }

//     for ($i = 0; $i < $generationCount; $i++) {
//         // 计算适应度  
//         $fitness = [];
//         foreach ($population as $index => $individual) {
//             $fitness[$index] = calculateFitness($individual, $polygons, $targetArea);
//         }

//         // 选择操作  
//         $elites = array_slice($population, 0, $eliteCount);
//         $parents = [];
//         while (count($parents) < $populationSize) {
//             $fitnessSum = array_sum($fitness);
//             $rands = rand(0, $fitnessSum, count($fitness));
//             foreach ($fitness as $index => $fitnessValue) {
//                 $randSum = 0;
//                 foreach ($rands as $r) {
//                     $randSum += ($fitnessValue / $fitnessSum) + rand() / count($rands);
//                 }
//                 if ($randSum > rand() / count($rands)) {
//                     $parents[] = $population[$index];
//                 }
//             }
//         }

//         // 交叉操作
//         $children = [];
//         for ($i = 0; $i < $populationSize; $i++) {
//             $parent1 = rand(0, count($parents) - 1);
//             $parent2 = rand(0, count($parents) - 1);
//             $child = [];
//             for ($j = 0; $j < count($parents[$parent1]); $j++) {
//                 if (rand() % 2 == 0) {
//                     $child[] = $parents[$parent1][$j];
//                 } else {
//                     $child[] = $parents[$parent2][$j];
//                 }
//             }
//             while (count($child) < count($polygons)) {
//                 $x = rand(0, $targetArea - $polygons[$i]->area);
//                 $y = rand(0, $targetArea - $polygons[$i]->area);
//                 $child[] = [$x, $y];
//             }
//             $children[] = $child;
//         }

//         // 变异操作  
//         foreach ($children as $index => $child) {
//             if (rand() / count($parents) < $mutationChance) {
//                 $x = rand(0, $targetArea - $polygons[$i]->area);
//                 $y = rand(0, $targetArea - $polygons[$i]->area);
//                 $child[rand(0, count($child) - 1)] = [$x, $y];
//             }
//         }

//         // 更新种群  
//         $population = $parents + $children;
//     }

//     // 找到最优解并输出结果  
//     $maxFitness = 0;
//     $bestIndividual = null;
//     foreach ($population as $individual) {
//         $fitness = calculateFitness($individual, $polygons, $targetArea);
//         if ($fitness > $maxFitness) {
//             $maxFitness = $fitness;
//             $bestIndividual = $individual;
//         }
//     }

//     // 输出最终的每个多边形的点坐标  
//     foreach ($bestIndividual as $index => $point) {
//         $polygon = $polygons[$index];
//         echo "Polygon " . ($index + 1) . ":\n";
//         echo "Points: ";
//         foreach ($polygon->points as $p) {
//             echo "(" . $p[0] . ", " . $p[1] . ") ";
//         }
//         echo "\n";
//     }
// }


function geneticSqueeze($polygons, $targetArea,$image)
{
    $populationSize = 100;
    $generationCount = 100;
    $eliteCount = 10;
    $mutationChance = 0.1;
  
    $population = [];
    while (count($population) < $populationSize) {
        $individual = [];
        foreach ($polygons as $polygon) {
            $x = rand(0, $targetArea - $polygon->area);
            $y = rand(0, $targetArea - $polygon->area);
            $individual[] = [$x, $y];
        }
        $population[] = $individual;
    }

    for ($i = 0; $i < $generationCount; $i++) {
        // 计算适应度  
        $fitness = [];
        foreach ($population as $index => $individual) {
            $fitness[$index] = calculateFitness($individual, $polygons, $targetArea);
        }


        // 选择操作  
        $elites = array_slice($population, 0, $eliteCount);
        $parents = [];
        while (count($parents) < $populationSize) {
            $parent1 = rand(0, count($elites) - 1);
            $parent2 = rand(0, count($elites) - 1);
            $child = [];
            for ($j = 0; $j < count($elites[$parent1]); $j++) {
                if (rand() % 2 == 0) {
                    $child[] = $elites[$parent1][$j];
                } else {
                    $child[] = $elites[$parent2][$j];
                }
            }
            while (count($child) < count($polygons)) {
                $x = rand(0, $targetArea - $polygons[$i]->area);
                $y = rand(0, $targetArea - $polygons[$i]->area);
                $child[rand(0, count($child) - 1)] = [$x, $y];
            }
            $parents[] = $child;
        }

        // 交叉操作  
        $children = [];
        for ($i = 0; $i < $populationSize; $i++) {
            $parent1 = rand(0, count($parents) - 1);
            $parent2 = rand(0, count($parents) - 1);
            $child = [];
            for ($j = 0; $j < count($parents[$parent1]); $j++) {
                if (rand() % 2 == 0) {
                    $child[] = $parents[$parent1][$j];
                } else {
                    $child[] = $parents[$parent2][$j];
                }
            }
            while (count($child) < count($polygons)) {
                $x = rand(0, $targetArea - $polygons[$i]->area);
                $y = rand(0, $targetArea - $polygons[$i]->area);
                $child[rand(0, count($child) - 1)] = [$x, $y];
            }
            $children[] = $child;
        }

        // 变异操作  
        foreach ($children as $index => $child) {
            if (rand() / count($parents) < $mutationChance) {
                $x = rand(0, $targetArea - $polygons[$i]->area);
                $y = rand(0, $targetArea - $polygons[$i]->area);
                $child[rand(0, count($child) - 1)] = [$x, $y];
            }
        }

        // 更新种群  
        $population = $parents + $children;
    }

    // 找到最优解并输出结果  
    $maxFitness = 0;
    $bestIndividual = null;
    foreach ($population as $individual) {
        $fitness = calculateFitness($individual, $polygons, $targetArea);
        if ($fitness > $maxFitness) {
            $maxFitness = $fitness;
            $bestIndividual = $individual;
        }
    }

    // 输出最终的每个多边形的点坐标  
    foreach ($bestIndividual as $index => $point) {
        $polygon = $polygons[$index];
        echo "Polygon " . ($index + 1) . ":\n";
        echo "Points: ";
        $polygon->draw($image->getres());
        foreach ($polygon->points as $p) {
            echo "(" . $p[0] . ", " . $p[1] . ") ";
        }
        echo "\n";
       
    }
}









$image = new image(50, 50);
$polygons = [];
for ($i = 0; $i < 8; $i++) {
    $points = [];
    for ($j = 0; $j < 4; $j++) {
        $x = rand(-20, 20);
        $y = rand(-20, 20);
        $points[] = [$x, $y];
    }

    $polygon = new Polygon($points);
    $polygons[] = $polygon;
}
// $polygons=[
//     new Polygon ( [
//         [0,0],[0,10],[1,10],[10,0],
//     ]),
//     new Polygon ( [
//         [0,0],[0,300],[1,100],[100,0],
//     ]),
//     new Polygon ( [
//         [0,0],[0,400],[1,200],[100,0],
//     ])
// ];
// p($polygons);
// 执行遗传挤压算法  
geneticSqueeze($polygons, 500,$image );
// foreach ($polygons as $key => $value) {
//     # code...
//     $value->draw($image->getres());
// }
$imgres = $image->save();

echo '  <script src="http://www.ng169.com/tpl/static/js/encode/jquery.min.js" type="text/javascript"></script>
<script src="http://www.ng169.com/tpl/static/js/night_Trad.v1.0.js" type="text/javascript"></script>';
echo "<img style='width:250px' onclick='_go_url_new(\"" . $imgres . "\")' src='" . $imgres . "'>";

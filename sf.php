<?php


function d($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    // exit;
}


// 定义多边形类  
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
    //绘制图形
    public function draw($image,$offset){
      
        // $image = imagecreatetruecolor(1000, 1000);
        $text_color = $color ?? imagecolorallocate($image, 255, 255, 255);
        imagefilledpolygon($image, $this->points, 4, $text_color);

    }
}




// 定义遗传挤压算法类  
class GeneticSqueezingAlgorithm
{
    private $polygons;
    private $population_size;
    private $iterations;
    private $PHP_INFINITY;
    public function __construct($polygons, $population_size, $iterations)
    {
        $this->polygons = $polygons;
        $this->population_size = $population_size;
        $this->iterations = $iterations;
        $this->PHP_INFINITY = log(0);
    }

    // 选择函数，使用轮盘赌博算法选择两个个体进行交叉和变异  
    function select($population)
    {
        $fitness_sum = 0;
        $best_poly = null;
        // $PHP_INFINITY = log(0);
        $best_fitness = -$this->PHP_INFINITY;

        foreach ($population as $i => $poly) {
            if (!isset($this->polygons[$i])) break;
            $area = $this->polygons[$i]->area();

            $fitness = -$area;
            $fitness_sum += $fitness;
            if ($fitness > $best_fitness) {
                $best_fitness = $fitness;
                $best_poly = $poly;
            }
        }
        $rand = rand() / rand();
        $acc = 0;
        foreach ($population as $i => $poly) {
            if (!isset($this->polygons[$i])) break;
            $area = $this->polygons[$i]->area();
            $probability = $fitness_sum == 0 ? 1 : abs($fitness[$i] / $fitness_sum);
            $acc += $probability;
        }
        $rand = $rand * $acc;
        $index1 = floor($rand);
        return [$population[$index1], $population[($index1 + 1) % count($population)]];
    }

    // 交叉函数，将两个多边形挤压在一起并生成两个新多边形  
    function crossover($parent1, $parent2)
    {
        $child1 = [];
        $child2 = [];
        $count = count($parent1);
        $mid = intval($count / 2);

        for ($i = 0; $i < $mid; $i++) {
            $child1[] = $parent1[$i];
            $child2[] = $parent2[$i];
        }

        for ($i = $mid; $i < $count; $i++) {
            $child1[] = $parent2[$i];
            $child2[] = $parent1[$i];
        }

        return [$child1, $child2];
    }

    // 变异函数，对多边形的某个顶点进行随机偏移  
    function mutate($poly, $mutation_rate)
    {
        $count = count($poly);

        for ($i = 0; $i < $count; $i++) {
            if (rand() / rand() < $mutation_rate) {
                $poly[$i] = [$poly[$i][0] + rand(-5, 5), $poly[$i][1] + rand(-5, 5)];
            }
        }

        return $poly;
    }

    // 进行遗传挤压算法优化，返回最优多边形顶点坐标数组  
    function optimize()
    {
        $population = [];
        for ($i = 0; $i < $this->population_size; $i++) {
            $poly = [];
            for ($j = 0; $j < count($this->polygons); $j++) {
                $x = rand(-25, 25);
                $y = rand(-25, 25);
                $poly[] = [$x, $y];
            }
            $population[] = $poly;
        }

        for ($i = 0; $i < $this->iterations; $i++) {
            // 选择两个个体进行交叉和变异  
            $parent1 = $this->select($population);
            $parent2 = $this->select($population);
            list($child1, $child2) = $this->crossover($parent1[0], $parent2[0]);
            $child1 = $this->mutate($child1, 0.2);
            $child2 = $this->mutate($child2, 0.2);

            // 将新个体加入种群  
            $population[] = $child1;
            $population[] = $child2;

            // 计算适应度  
            $fitness = [];
            foreach ($population as $poly) {
                $area = 0;
                for ($j = 0; $j < count($poly); $j++) {
                    $p1 = $poly[$j];
                    $p2 = $poly[(($j + 1) % count($poly))];
                    $area += min($p1[0], $p2[0]) * min($p1[1], $p2[1]) * 0.5;
                }
                $fitness[] = -$area;
            }

            // 选择两个个体进行替换  
            $index1 = rand(0, count($population) - 1);
            $index2 = rand(0, count($population) - 1);
            if ($fitness[$index1] > $fitness[$index2]) {
                $population[$index2] = $population[$index1];
            } else {
                $population[$index1] = $population[$index2];
            }
        }

        // 返回最优多边形顶点坐标数组  
        $fitness = [];
        foreach ($population as $poly) {
            $area = 0;
            for ($j = 0; $j < count($poly); $j++) {
                $p1 = $poly[$j];
                $p2 = $poly[(($j + 1) % count($poly))];
                $area += min($p1[0], $p2[0]) * min($p1[1], $p2[1]) * 0.5;
            }
            $fitness[] = -$area;
        }
        $best_index = array_search(max($fitness), $fitness);
        return $population[$best_index];
    }
    //绘制
    private  $image;
    public function draw(){
        $this->image = imagecreatetruecolor(1000, 1000);
        foreach ($variable as $key => $value) {
            # code...
        }
    }

    public function saveimg(){
        $str = "pic/" . rand(111, 999999) . ".png";
        imagepng($this->image, $str);
        // 释放内存  
        imagedestroy($this->image);


        return $str;
    }
}

// 示例用法  
$polygons = [
    new Polygon([0, 0, 1, 0, 1, 1, 0, 1]), // 正方形  
    new Polygon([0, 0, 1, 0, 0, 1, 0.5, 0.5]), // 菱形  
    new Polygon([0, 0, 1, 0, -1, 0, -1, -1, 0, -1]), // 等腰直角三角形  
    new Polygon([-1, -1, -0.5, -1, -0.5, 0, 0.5, 0, 0.5, -1]), // 箭头形  
];

$ga = new GeneticSqueezingAlgorithm($polygons, 50, 100);
$result = $ga->optimize();

foreach ($result as $key => $value) {
    # code...
}
d($result);













































// //遗传挤压算法
// class GeneticAlgorithm
// {
//     // 定义算法参数  
//     private $pop_size = 50; // 种群大小  
//     private $iter_num = 100; // 迭代次数  
//     private $cross_rate = 0.7; // 交叉概率  
//     private $mut_rate = 0.1; // 变异概率  
//     private $push_num = 5; // 队列大小  
//     // 定义二维数组表示图形  
//     // 以上算法中的$map数组用于存储不规则图形的顶点坐标。每个元素都是一个数组，表示一个多边形的顶点坐标。例如，对于第一个不规则图形，$map数组的第一个元素可以表示为array(0, 0, 2, 0)，其中0, 0表示左上角顶点的坐标，2, 0表示右下角顶点的坐标。对于其他不规则图形，也可以按照类似的方式表示其顶点坐标。
//     private $map = array(
//         array(0, 0, 1, 1),
//         array(0, 1, 1, 2),
//         array(1, 1, 2, 2),
//         array(1, 2, 2, 3),
//         array(2, 2, 3, 3),
//         array(2, 3, 3, 4),
//     );
//     // 初始化种群  
//     private $pop = array();
//     // 定义队列  
//     private $queue = array();

//     //添加图形对象
//     private function  addbox()
//     {
//     }

//     public function calc()
//     {
//         for ($i = 0; $i < $this->pop_size; $i++) {
//             $x1 = rand(0, $this->map[0][2] - $this->map[0][0]);
//             $y1 = rand(0, $this->map[0][3] - $this->map[0][1]);
//             $x2 = rand(0, $this->map[0][2] - $this->map[0][0]);
//             $y2 = rand(0, $this->map[0][3] - $this->map[0][1]);
//             $this->pop[$i] = array($x1, $y1, $x2, $y2);
//         }
      
//         $this->_calc();
//     }
//     // 迭代优化过程  
//     private function _calc()
//     {

//         for ($i = 0; $i < $this->iter_num; $i++) {
//             // 选择前几个优秀的个体  
//             if (count($this->queue) < $this->push_num) {
//                 foreach ($this->pop as $p) {
//                     $this->queue[] = $p;
//                 }
//             } else {
//                 $index = array_rand($this->queue);
              
//                 $new_pop = array();
//                 for ($j = 0; $j < count($this->queue); $j++) {
//                     $p1 = $this->queue[$index[$j]];
//                     $p2 = $this->queue[$index[rand(0, count($this->queue) - 1)]];
//                     if (rand() < $this->cross_rate) {
//                         $x1 = rand($p1[0], $p1[2]);
//                         $y1 = rand($p1[1], $p1[3]);
//                         $x2 = rand($p2[0], $p2[2]);
//                         $y2 = rand($p2[1], $p2[3]);
//                         $new_pop[] = array($x1, $y1, $x2, $y2);
//                     } else {
//                         $x1 = rand($p1[0], $p1[2]);
//                         $y1 = rand($p1[1], $p1[3]);
//                         $x2 = rand($p1[0], $p1[2]);
//                         $y2 = rand($p1[1], $p1[3]);
//                         if (rand() < $this->mut_rate) {
//                             $x3 = rand($x1 - $p1[0], $x1 - $p1[2]);
//                             $y3 = rand($y1 - $p1[1], $y1 - $p1[3]);
//                             $x4 = rand($x2 - $p1[0], $x2 - $p1[2]);
//                             $y4 = rand($y2 - $p1[1], $y2 - $p1[3]);
//                             if ($x3 + $x4 > $this->map[0][2] - $this->map[0][0]) {
//                                 $x3 = $this->map[0][2] - $this->map[0][0] - ($x4 + $x3);
//                             }
//                             if ($y3 + $y4 > $this->map[0][3] - $this->map[0][1]) {
//                                 $y3 = $this->map[0][3] - $this->map[0][1] - ($y4 + $y3);
//                             }
//                             $new_pop[] = array($x1 + $x3, $y1 + $y3, $x2 + $x4, $y2 + $y4);
//                         }

//                         // 对新个体进行挤压操作  
//                         $new_pop_score = array();
//                         for ($j = 0; $j < count($new_pop); $j++) {
//                             $p = $new_pop[$j];
//                             $score = 0;
//                             for ($k = 0; $k < count($this->map); $k++) {
//                                 if ($p[0] >= $this->map[$k][0] && $p[1] >= $this->map[$k][1] && $p[2] <= $this->map[$k][2] && $p[3] <= $this->map[$k][3]) {
//                                     $score += pow($p[2] - $p[0], 2) + pow($p[3] - $p[1], 2);
//                                 }
//                             }
//                             $new_pop_score[$j] = $score;
//                         }

//                         // 将新个体加入队列  
//                         for ($j = 0; $j < count($new_pop); $j++) {
//                             $p = $new_pop[$j];
//                             $score = $new_pop_score[$j];
//                             $flag = 0;
//                             for ($k = 0; $k < count($this->queue); $k++) {
//                                 if ($p[0] >= $this->queue[$k][0] && $p[1] >= $this->queue[$k][1] && $p[2] <= $this->queue[$k][2] && $p[3] <= $this->queue[$k][3]) {
//                                     $flag = 1;
//                                     break;
//                                 }
//                             }
//                             if ($flag == 0) {
//                                 $this->queue[] = $p;
//                             }
//                         }

//                         // 输出当前的最优解  
//                         $best_score = PHP_INT_MAX;
//                         for ($j = 0; $j < count($this->queue); $j++) {
//                             $p = $this->queue[$j];
//                             $score = 0;
//                             for ($k = 0; $k < count($this->map); $k++) {
//                                 if ($p[0] >= $this->map[$k][0] && $p[1] >= $this->map[$k][1] && $p[2] <= $this->map[$k][2] && $p[3] <= $this->map[$k][3]) {
//                                     $score += pow($p[2] - $p[0], 2) + pow($p[3] - $p[1], 2);
//                                 }
//                             }
//                             if ($score < $best_score) {
//                                 $best_score = $score;
//                                 $best_pos = $p;
//                             }
//                         }
//                         echo "Best score: " . $best_score . "\n<p>";
//                         echo "Best position: [" . $best_pos[0] . ", " . $best_pos[1] . ", " . $best_pos[2] . ", " . $best_pos[3] . "]\n<p>";
//                     }
//                 }
//             }
//         }
//     }
// }
// (new GeneticAlgorithm())->calc();
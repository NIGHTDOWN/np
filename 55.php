<?php  
  function p($val)
  {
      // var_dump($val);
      var_export($val);
      echo "<br/>";
      // exit;
  }
// 最大矩形数量  
  
// 遗传挤压算法的个体和适应度函数  
class Individual {  

// 遗传挤压算法的参数  
private static $population_size = 100;    // 种群大小  
private static $generations = 50;        // 迭代次数  
private static $mutation_rate = 0.1;     // 变异率  
  
 // 矩 static 形尺寸和总面积  
private static $rect_width = 240;  
private static $rect_height = 299;  
private static $paper_width = 500;    // 大度纸宽度，假设为500mm  
private static $paper_height = 600;   // 大度纸长度，假设为600mm  
 
private static $total_area = 500 * 600;  
private static $max_count = 0;  




    public $genes;  
    public $fitness;  
  
    public function __construct() {  
        $this->genes = [];  
        $this->fitness = 0;  
    }  
  
    // 适应度函数  
    public function evaluate() {  
        $count = count($this->genes);  
        $fitness = $count * $this->get_fitness();  
        $this->fitness = $fitness;  
        return $fitness;  
    }  
  
   // 计算一个个体的适应度  
public function get_fitness() {  
    $x_sum = 0;  
    $y_sum = 0;  
    foreach ($this->genes as $gene) {  
        $x_sum += $gene[0];  
        $y_sum += $gene[1];  
    }  
    $x_avg = $x_sum / count($this->genes);  
    $y_avg = $y_sum / count($this->genes);  
    $total_fitness = 0;
    $overlap = 0;  
    foreach ($this->genes as $gene) {  
        $x = $gene[0];  
        $y = $gene[1];  
        $width = $this->rect_width;  
        $height = $this->rect_height;  
  
        // 计算当前矩形的面积和与另一个矩形的重叠面积  
        $area = $width * $height;  
        $overlap_area = max(0, intval(round($x - $width / 2)) - intval(round($x_avg - $width / 2))) * max(0, intval(round($y - $height / 2)) - intval(round($y_avg - $height / 2)));  
  
        // 计算当前矩形的适应度贡献  
        $fitness_contribution = $area - $overlap_area;  
        $overlap += $overlap_area;  
  
        // 计算所有矩形的适应度总和  
        $total_fitness += $fitness_contribution;  
    }  
  
    // 计算整个矩形的适应度，即所有矩形的适应度总和除以矩形数量  
    $fitness = $total_fitness / count($this->genes);  
    return $fitness;  
}  
  
    // 遗传挤压算法的交叉算子  
    public function crossover($other) {  
        $mid = floor($this->population_size / 2);  
        $child1 = clone $this;  
        $child2 = clone $other;  
  
        // 将两个个体的基因进行混合  
        for ($i = 0; $i < $mid; $i++) {  
            foreach ($this->genes as $gene) {  
                $child1->genes[] = $gene;  
            }  
            foreach ($other->genes as $gene) {  
                $child2->genes[] = $gene;  
            }  
        }  
  
        // 随机打乱其中一个个体的基因，以增加随机性  
        srand(time());  
        $permutation = [];  
        for ($i = 0; $i < count($child1->genes); $i++) {  
            $permutation[$i] = $i;  
        }  
        shuffle($permutation);  
        foreach ($permutation as $index) {  
            $child1->genes[$index] = $child2->genes[$index];  
        }  
  
        return [$child1, $child2];  
    }  
  
    // 遗传挤压算法的变异算子  
    public function mutate() {  
        $基因个数 = count($this->genes);  
        $indices = range(0, $基因个数 - 1);  
        shuffle($indices);  
  
        for ($i = 0; $i < (int)($this->mutation_rate * $基因个数); $i++) {  
            $index = $indices[$i];  
            $this->genes[$index] = [$this->genes[$index][0] + rand(-5, 5), $this->genes[$index][1] + rand(-5, 5)];  
            // 如果基因坐标超出了大度纸范围，则将其移到大度纸边缘  
            if ($this->genes[$index][0] < 0) {  
                $this->genes[$index][0] = -$this->rect_width;  
            } elseif ($this->genes[$index][0] > $this->paper_width - $this->rect_width) {  
                $this->genes[$index][0] = $this->paper_width - $this->rect_width;  
            }  
            if ($this->genes[$index][1] < 0) {  
                $this->genes[$index][1] = -$this->rect_height;  
            } elseif ($this->genes[$index][1] > $this->paper_height - $this->rect_height) {  
                $this->genes[$index][1] = $this->paper_height - $this->rect_height;  
            }
        }  
    }  
  
    // 遗传挤压算法的选择算子  
    public static function selection($population) {  
        p($population);
        // 根据适应度值排序  
        usort($population, function ($a, $b) {  
            return $b->fitness - $a->fitness;  
        });  
  
        $new_population = [];  
        // 选择前一半的个体，即最好的50%  
        for ($i = 0; $i < floor(self::$population_size / 2); $i++) {  
            $new_population[] = $population[$i];  
        }  
        // 将最好的两个个体进行交叉，产生两个新个体  
        for ($i = 0; $i < floor(self::$population_size / 2); $i++) {  
            $pair = $population[$i];  
         
            list($child1, $child2) = $pair->crossover($population[$i + 1]);  
            $new_population[] = $child1;  
            $new_population[] = $child2;  
        }  
  
        return $new_population;  
    }  
  
    // 遗传挤压算法的主函数  
    public static function evolution($population) {  
        for ($i = 0; $i < self::$generations; $i++) {  
            // 选择最好的两个个体进行交叉，产生新的个体  
            $new_population = [];  
            while (count($new_population) < self::$population_size) {  
                $pairs = array_map(function ($individual) {  
                    return [$individual, self::mutate($individual)];  
                }, self::selection($population));  
                foreach ($pairs as $pair) {  
                    $new_population[] = $pair[0];  
                    $new_population[] = $pair[1];  
                }  
            }  
            $population = $new_population;  
        }  
        return $population;  
    }  
}
// 初始化种群，每个个体的基因表示其在矩形中的位置  
$population = [];  
for ($i = 0; $i < $population_size; $i++) {  
    $individual = new Individual();  
    for ($j = 0; $j < 5; $j++) {  
        $x = rand(0, $paper_width - $rect_width);  
        $y = rand(0, $paper_height - $rect_height);  
        $individual->genes[] = [$x, $y];  
    }  
    $population[] = $individual;  
}  
  p($individual);
// 进行遗传挤压算法的进化过程  
$population = $individual->evolution($population);  
  
// 输出结果，将每个矩形的位置打印在纸上  
foreach ($population as $individual) {  
    foreach ($individual->genes as $gene) {  
        echo "{$gene[0]}, {$gene[1]}\n";  
    }  
}  
?>
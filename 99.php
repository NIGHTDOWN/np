<?php
function p($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    // exit;
}
// 定义矩形的宽度和高度
$rectWidth = 240;
$rectHeight = 299;

// 定义纸的宽度和高度
$paperWidth = 889;
$paperHeight = 1194;

// 计算最大矩形数量
$maxRectangleCount = floor($paperWidth / $rectWidth) * floor($paperHeight / $rectHeight);

// 初始化种群大小
$populationSize = 100;

// 生成初始种群
$population = array();
for ($i = 0; $i < $populationSize; $i++) {
    $solution = array();
    for ($j = 0; $j < $maxRectangleCount; $j++) {
        $solution[] = rand(0, 1) == 0 ? false : true;
    }
    $population[] = $solution;
}
p($population);
// 计算适应度函数（利用率）
function fitness($solution) {
    $rectangles = array();
    $x = 0;
    $y = 0;
    for ($i = 0; $i < count($solution); $i++) {
        if ($solution[$i]) {
            $rectangles[] = array('x' => $x, 'y' => $y);
            $x += 240;
            if ($x >= 889) {
                $x = 0;
                $y += 299;
            }
        }
    }
    $areaSum = 0;
    foreach ($rectangles as $rectangle) {
        $areaSum += $rectangle['x'] * $rectangle['y'];
    }
    return $areaSum / (889 * 1194);
}

// 进化过程
$generationCount = 100;
$mutationRate = 0.1;
for ($generation = 0; $generation < $generationCount; $generation++) {
    // 计算适应度
    $fitnesses = array();
    for ($i = 0; $i < $populationSize; $i++) {
        $fitnesses[$i] = fitness($population[$i]);
    }

    // 选择父代
    $parents = array();
    for ($i = 0; $i < $populationSize / 2; $i++) {
        $index1 = array_search(max($fitnesses), $fitnesses);
        $fitnesses[$index1] = -1;
        $index2 = array_search(max($fitnesses), $fitnesses);
        $fitnesses[$index2] = -1;
        $parents[$i] = array($population[$index1], $population[$index2]);
    }

    // 生成子代
    $children = array();
    for ($i = 0; $i < count($parents); $i++) {
        $parent1 = $parents[$i][0];
        $parent2 = $parents[$i][1];
        $child1 = array();
        $child2 = array();
        for ($j = 0; $j < $maxRectangleCount; $j++) {
            if ($j % 2 == 0) {
                $child1[] = $parent1[$j];
                $child2[] = $parent2[$j];
            } else {
                $child1[] = $parent2[$j];
                $child2[] = $parent1[$j];
            }
        }
        $children[] = $child1;
        $children[] = $child2;
    }

    // 变异
    for ($i = 0; $i < count($children); $i++) {
        for ($j = 0; $j < $maxRectangleCount; $j++) {
            if (mt_rand() / mt_getrandmax() < $mutationRate) {
                $children[$i][$j] = !$children[$i][$j];
            }
        }
    }

    // 生成新一代种群
    $population = array_merge($parents, $children);
}

// 找出最优解
$maxFitness = 0;
$maxFitnessIndex = 0;
for ($i = 0; $i < $populationSize; $i++) {
    $fitness = fitness($population[$i]);
    if ($fitness > $maxFitness) {
        $maxFitness = $fitness;
        $maxFitnessIndex = $i;
    }
}

// 输出结果
$bestSolution = $population[$maxFitnessIndex];
$rectangleCount = 0;
$rectangles = array();
$x = 0;
$y = 0;
for ($i = 0; $i < count($bestSolution); $i++) {
    if ($bestSolution[$i]) {
        $rectangles[] = array('x' => $x, 'y' => $y);
        $x += $rectWidth;
        if ($x >= $paperWidth) {
            $x = 0;
            $y += $rectHeight;
        }
        $rectangleCount++;
    }
}

echo "最优解的排列数量：$rectangleCount\n";
echo "每个矩形的坐标：\n";
for ($i = 0; $i < $rectangleCount; $i++) {
    $rectangle = $rectangles[$i];
    $x = $rectangle['x'];
    $y = $rectangle['y'];
    echo "矩形$i：($x, $y)\n";
}

?>
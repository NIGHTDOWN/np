import random  
  
# 矩形大小  
RECT_WIDTH = 240  
RECT_HEIGHT = 299  
  
# 纸张大小  
PAPER_WIDTH = 889  
PAPER_height = 1194  
  
# 种群大小  
POPULATION_SIZE = 50  
  
# 迭代次数  
GENERATIONS = 100  
  
# 遗传算法优化函数  
def ga_optimize(bounds):  
    # 初始化种群  
    population = []  
    for i in range(POPULATION_SIZE):  
        layout = []  
        for j in range(50):  
            x = random.randint(0, PAPER_width - RECT_WIDTH)  
            y = random.randint(0, PAPER_height - RECT_HEIGHT)  
            layout.append((x, y))  
        population.append(layout)  
  
    # 进化过程  
    for g in range(GENERATIONS):  
        # 计算适应度  
        fitness = [calculate_fitness(layout) for layout in population]  
  
        # 选择操作  
        selected = []  
        while len(selected) < POPULATION_SIZE):  
            father = random.choice(population)  
            mother = random.choice(population)  
            if father != mother:  
                selected.append((father, mother))  
  
        # 交叉操作  
        offspring = []  
        for i in range(POPULATION_SIZE):  
            parent1 = selected[i][0]  
            parent2 = selected[i][1]  
            if random.random() < 0.5:  
                child = crossover(parent1, parent2)  
            else:  
                child = parent1  
            offspring.append(child)  
  
        # 挤压操作  
        for i in range(POPULATION_SIZE):  
            layout = offspring[i]  
            for j in range(len(layout)):  
                x, y = layout[j]  
                dx = random.uniform(-10, 10)  
                dy = random.uniform(-10, 10)  
                if (x + dx, y + dy) not in layout:  
                    layout[j] = (x + dx, y + dy)  
  
        # 更新种群  
        population = offspring  
  
    # 返回最优解  
    best_fitness = min(fitness)  
    for i in range(POPULATION_SIZE):  
        if fitness[i] == best_fitness:  
            return population[i]  
  
# 计算适应度函数  
def calculate_fitness(layout):  
    x_counts = [0] * PAPER_width // RECT_WIDTH  
    y_counts = [0] * PAPER_height // RECT_HEIGHT  
    for (x, y) in layout:  
        x_counts[x // RECT_WIDTH] += 1  
        y_counts[y // RECT_HEIGHT] += 1  
    fitness = sum([min(x_counts[i], y_counts[i]) for i in range(PAPER_width // RECT_WIDTH)]) * PAPER_width // RECT_WIDTH * PAPER_height // RECT_HEIGHT  
    return fitness


    # 交叉函数  
def crossover(parent1, parent2):  
    layout = []  
    used = set()  
    start = random.randint(0, PAPER_width - RECT_WIDTH)  
    end = random.randint(0, PAPER_width - RECT_WIDTH)  
    while len(layout) < 50:  
        if end >= start:  
            x1, y1 = parent1[random.randint(start, end)]  
            x2, y2 = parent2[random.randint(start, end)]  
        else:  
            x1, y1 = parent1[random.randint(0, PAPER_width // RECT_WIDTH - 1)]  
            x2, y2 = parent2[random.randint(0, PAPER_width // RECT_WIDTH - 1)]  
        if (x1, y1) not in used and (x2, y2) not in used:  
            layout.append((x1, y1))  
            used.add((x1, y1))  
            used.add((x2, y2))  
        if len(layout) == 50:  
            break  
    return layout  
  
# 主函数  
if __name__ == "__main__":  
    bounds = [(0, PAPER_width), (0, PAPER_height)]  
    layout = ga_optimize(bounds)  
    print("最优解为：", layout)
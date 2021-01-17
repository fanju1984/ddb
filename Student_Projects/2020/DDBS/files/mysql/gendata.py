from faker import Faker
import random
import numpy as np
import csv

f = Faker(locale="en_US")

file = open('publisher.csv', 'w', encoding='utf-8', newline="")
csv_writer = csv.writer(file)
csv_writer.writerow(["id","name","nation"])
publisher = []
for i in range(100001, 105001):
    tmp = []
    tmp.append(i)
    tmp.append(f.name())
    if i <= 102500:
        tmp.append("USA")
    else:
        tmp.append("PRC")
    publisher.append(tmp)
    csv_writer.writerow(tmp)
file.close()

file = open('customer.csv', 'w', encoding='utf-8', newline="")
csv_writer = csv.writer(file)
csv_writer.writerow(["id","name","rank"])
customer = []
for i in range(300001, 315001):
    tmp = []
    tmp.append(i)
    tmp.append(f.name())
    if i <= 306000:
        tmp.append(1)
    elif i <= 310500:
        tmp.append(2)
    else:
        tmp.append(3)
    customer.append(tmp)
    csv_writer.writerow(tmp)
file.close()

file = open('book.csv', 'w', encoding='utf-8', newline="")
csv_writer = csv.writer(file)
csv_writer.writerow(["id","title","authors","publisher_id","copies"])
book = []
for i in range(200001, 250001):
    tmp = []
    tmp.append(i)
    tmp.append(f.sentence())
    tmp.append(f.name())
    tmp.append(random.randint(100001, 105000))
    tmp.append(random.randint(0, 10000))
    book.append(tmp)
    csv_writer.writerow(tmp)
file.close()

file = open('orders.csv', 'w', encoding='utf-8', newline="")
csv_writer = csv.writer(file)
csv_writer.writerow(["customer_id","book_id","quantity"])
mu, sigma, size = 3, 2, 100000
orders = []
for i in range(size):
    tmp = []
    tmp.append(random.randint(300001, 315000))
    tmp.append(random.randint(200001, 250000))
    while True:
        res = int(np.random.normal(mu, sigma, 1))
        if res > 0:
            break
    tmp.append(res)
    orders.append(tmp)
    csv_writer.writerow(tmp)
file.close()
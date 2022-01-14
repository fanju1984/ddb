python servermaster.py 8883 db1 >> log1 &
python servermaster.py 8885 db2 >> log2 &
ssh ddb2 
cd DDB_RPQL/server/ && python servermaster.py 8883 db1 >> log &
ssh ddb3 
cd DDB_RPQL/server/ && python servermaster.py 8883 db1 >> log &
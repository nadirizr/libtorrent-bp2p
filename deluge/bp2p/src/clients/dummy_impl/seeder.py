from betterp2p.network.simple import SimpleNetwork
import logging


class Seeder(object):
    
    def __init__(self):
        self.network = SimpleNetwork()
    
    def main(self):
        logging.root.setLevel(logging.INFO)
        logging.getLogger("bp2p.net").setLevel(logging.INFO)
        logging.info("Seeder")
        self.network.mainloop()


if __name__ == '__main__':
    Seeder().main()

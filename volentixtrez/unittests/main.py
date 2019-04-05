import unittest
from eosfactory.eosf import *
from settings import *
import sys
verbosity(VERBOSITY)

CONTRACT_WORKSPACE = sys.path[0] + "/../"

class Test(unittest.TestCase):
    '''
    volentixtrez smart-contract unittests.
    '''
    @classmethod
    def setUpClass(cls):
        reset()
        create_master_account("MASTER")
        create_account("volentixtrez", MASTER, "volentixtrez" )
        create_account("treasury", MASTER, "staider11111")
        create_account("volentixgsys", MASTER, "volentixgsys")
        create_account("tester", MASTER, "tester")
        
        volentixtrez_contract = Contract(volentixtrez, CONTRACT_WORKSPACE)
        volentixtrez_contract.build(force=False)
        volentixtrez_contract.deploy()

        token_contract = Contract(volentixgsys, "/usr/local/eosfactory/contracts/eosio_token")
        token_contract.build(force=False)
        token_contract.deploy()

        volentixgsys.push_action(
            "create", 
            {
                "issuer": volentixgsys,
                "maximum_supply": "500000000.00000000 VTX"
            }, [volentixgsys])

        volentixgsys.push_action(
            "issue", 
            {
                "to": tester,
                "quantity": "1000.00000000 VTX",
                "memo": ""
            }, [volentixgsys])
    

        tester.set_account_permission(
            permission_name="active",
            parent_permission_name="owner",
            authority = {
                "threshold": 1,
                "keys": [
                        {
                            "key": tester.active(),
                            "weight": 1        
                        }
                ],
                "accounts": [
                        {
                            "permission": 
                                {
                                    "actor": "volentixtrez",
                                    "permission": "eosio.code"
                                },
                            "weight": 1
                        }
                ]
            }
        )

        treasury.set_account_permission(
            permission_name="active",
            parent_permission_name="owner",
            authority = {
                "threshold": 1,
                "keys": [
                        {
                            "key": treasury.active(),
                            "weight": 1        
                        }
                ],
                "accounts": [
                        {
                            "permission": 
                                {
                                    "actor": "volentixtrez",
                                    "permission": "eosio.code"
                                },
                            "weight": 1
                        }
                ]
            }
        )    

    @staticmethod
    def get_vtx_balance(account):
            table = volentixgsys.table("accounts", account)
            return table.json["rows"][0]["balance"]

    def test_payfee_action(self):
        COMMENT('''
            Transfer 100 VTX from tester to treasury.
        ''')

        volentixtrez.push_action(
            "payfee",
            {
                "account": tester,
                "quantity": "100.00000000 VTX"
            },
            [treasury, tester])
        
        self.assertEqual(
            self.get_vtx_balance(treasury),
            "100.00000000 VTX",
            "wrong balance"
            )

        COMMENT(f'''
        Trying to invoke transfer action without treasury authority. 
        ''')
        with self.assertRaises(errors.MissingRequiredAuthorityError):
            volentixtrez.push_action(
                "payfee",
                {
                    "account": tester,
                    "quantity": "150.00000000 VTX"
                },
                [tester])

    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()

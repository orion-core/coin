# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *liborion_cli*         | RPC client functionality used by *orion-cli* executable |
| *liborion_common*      | Home for common functionality shared by different executables and libraries. Similar to *liborion_util*, but higher-level (see [Dependencies](#dependencies)). |
| *liborion_consensus*   | Stable, backwards-compatible consensus functionality used by *liborion_node* and *liborion_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *liborionconsensus*    | Shared library build of static *liborion_consensus* library |
| *liborion_kernel*      | Consensus engine and support library used for validation by *liborion_node* and also exposed as a [shared library](../shared-libraries.md). |
| *liborionqt*           | GUI functionality used by *orion-qt* and *orion-gui* executables |
| *liborion_ipc*         | IPC functionality used by *orion-node*, *orion-wallet*, *orion-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *liborion_node*        | P2P and RPC server functionality used by *oriond* and *orion-qt* executables. |
| *liborion_util*        | Home for common functionality shared by different executables and libraries. Similar to *liborion_common*, but lower-level (see [Dependencies](#dependencies)). |
| *liborion_wallet*      | Wallet functionality used by *oriond* and *orion-wallet* executables. |
| *liborion_wallet_tool* | Lower-level wallet functionality used by *orion-wallet* executable. |
| *liborion_zmq*         | [ZeroMQ](../zmq.md) functionality used by *oriond* and *orion-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *liborion_consensus* and *liborion_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`liborion_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *liborion_node* code lives in `src/node/` in the `node::` namespace
  - *liborion_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *liborion_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *liborion_util* code lives in `src/util/` in the `util::` namespace
  - *liborion_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

orion-cli[orion-cli]-->liborion_cli;

oriond[oriond]-->liborion_node;
oriond[oriond]-->liborion_wallet;

orion-qt[orion-qt]-->liborion_node;
orion-qt[orion-qt]-->liborionqt;
orion-qt[orion-qt]-->liborion_wallet;

orion-wallet[orion-wallet]-->liborion_wallet;
orion-wallet[orion-wallet]-->liborion_wallet_tool;

liborion_cli-->liborion_util;
liborion_cli-->liborion_common;

liborion_common-->liborion_consensus;
liborion_common-->liborion_util;

liborion_kernel-->liborion_consensus;
liborion_kernel-->liborion_util;

liborion_node-->liborion_consensus;
liborion_node-->liborion_kernel;
liborion_node-->liborion_common;
liborion_node-->liborion_util;

liborionqt-->liborion_common;
liborionqt-->liborion_util;

liborion_wallet-->liborion_common;
liborion_wallet-->liborion_util;

liborion_wallet_tool-->liborion_wallet;
liborion_wallet_tool-->liborion_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class orion-qt,oriond,orion-cli,orion-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *liborion_wallet* and *liborion_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *liborion_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *liborion_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *liborion_common* should serve a similar function as *liborion_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *liborion_util* and *liborion_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for orion-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *liborion_kernel* is not supposed to depend on *liborion_common*, only *liborion_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *liborion_kernel* should only depend on *liborion_util* and *liborion_consensus*.

- The only thing that should depend on *liborion_kernel* internally should be *liborion_node*. GUI and wallet libraries *liborionqt* and *liborion_wallet* in particular should not depend on *liborion_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *liborion_consensus*, *liborion_common*, and *liborion_util*, instead of *liborion_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *liborionqt*, *liborion_node*, *liborion_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *liborion_node* to *liborion_kernel* as part of [The liborionkernel Project #24303](https://github.com/orion/orion/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/orion/orion/issues/15732)

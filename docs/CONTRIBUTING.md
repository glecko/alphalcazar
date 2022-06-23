## Adding documentation

### C++

API documentation for C++ code should be written in standard [Doxygen style](https://www.doxygen.nl/manual/docblocks.html).
Use `/*!` for multiline blocks and `///` for single line.

Example:
```c++
/*!
 * \brief A class for teleporting bread.
 *
 * \warning This API is experimental and its effects on the bread have not been tested!
 */
class BreadTeleporter {
public:
     BreadTeleporter();

     /// Returns the amount of times the teleporter has been used
     std::uint32_t GetUseCount() const;

     /*!
     * \brief Teleports the bread of a given ID
     * \param breadId The ID of the bread to teleport
     * \param distance The distance we want to teleport the bread away.
     * \returns True if the bread was teleported successfully.
     *
     * \note While negative distances are allowed for \param distance, teleporting a bread a negative distance
     *       may have unforseen consequences.
     */
     bool TeleportBread(std::uint64_t breadId, std::int64_t distance);
private:
     /// The expiration date of the last teleported bread
     std::chrono::system_clock::time_point mExpirationDate;
};
```
